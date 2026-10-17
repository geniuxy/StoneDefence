// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_DrawWeapon.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "Tags/StatsTags.h"

UGA_DrawWeapon::UGA_DrawWeapon()
{
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_EquippingWeapon);
	// ActivationRequiredTags.AddTag(StatsTags::Ge_Stats_EquippingSword);
}

void UGA_DrawWeapon::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 基类 ActivateAbility 在 PreLoadMontage() 失败时会 K2_EndAbility()，
	// 但它不会 return —— 派生类必须自己判活，否则会在一个已经结束的能力上继续建任务。
	if (!IsActive())
	{
		return;
	}

	if (!K2_CommitAbility() || !GetOwnerASC())
	{
		K2_EndAbility();
		return;
	}

	bStanceNeedsCleanup = true;

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayDrawSwordMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DrawSwordMontage);
		PlayDrawSwordMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayDrawSwordMontage->ReadyForActivation();
	}

	ArmInputWait();
}

void UGA_DrawWeapon::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ClearStanceEffects();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_DrawWeapon::ArmInputWait()
{
	if (WaitInputPress)
	{
		WaitInputPress->EndTask();
		WaitInputPress = nullptr;
	}

	WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

bool UGA_DrawWeapon::PreLoadMontage()
{
	if (!DrawSwordMontageTag.IsValid() || !SheatheSwordMontageTag.IsValid())
	{
		return false;
	}
	const TSoftObjectPtr<UAnimMontage> DrawMontage = GetMontageByTag(DrawSwordMontageTag);
	const TSoftObjectPtr<UAnimMontage> SheatheMontage = GetMontageByTag(SheatheSwordMontageTag);
	if (DrawMontage.IsNull() || SheatheMontage.IsNull())
	{
		return false;
	}

	if (GetOwnerASC() && GetOwnerASC()->AreMontagesLoaded())
	{
		DrawSwordMontage = GetMontageByTag(DrawSwordMontageTag).Get();
		SheatheSwordMontage = GetMontageByTag(SheatheSwordMontageTag).Get();
		return DrawSwordMontage != nullptr && SheatheSwordMontage != nullptr;
	}

	DrawSwordMontage = DrawMontage.LoadSynchronous();
	SheatheSwordMontage = SheatheMontage.LoadSynchronous();

	return DrawSwordMontage != nullptr && SheatheSwordMontage != nullptr;
}

void UGA_DrawWeapon::HandleInputPress(float TimeWaited)
{
	if (!GetOwnerASC() || !GetOwnerASC()->HasMatchingGameplayTag(StatsTags::Ge_Stats_InCombat))
	{
		ArmInputWait();
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlaySheatheSwordMontage =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, SheatheSwordMontage);
	PlaySheatheSwordMontage->OnCompleted.AddDynamic(this, &ThisClass::OnSheatheSwordEnd);
	PlaySheatheSwordMontage->OnBlendOut.AddDynamic(this, &ThisClass::OnSheatheSwordEnd);
	PlaySheatheSwordMontage->OnInterrupted.AddDynamic(this, &ThisClass::OnSheatheSwordEnd);
	PlaySheatheSwordMontage->OnCancelled.AddDynamic(this, &ThisClass::OnSheatheSwordEnd);
	PlaySheatheSwordMontage->ReadyForActivation();
}

void UGA_DrawWeapon::OnSheatheSwordEnd()
{
	ClearStanceEffects();
	K2_EndAbility();
}

void UGA_DrawWeapon::ClearStanceEffects()
{
	// OnSheatheSwordEnd 和 EndAbility 都可能进来，做成幂等的。
	if (!bStanceNeedsCleanup)
	{
		return;
	}
	bStanceNeedsCleanup = false;

	UGeAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC)
	{
		return;
	}

	ASC->RemoveActiveEffectsWithGrantedTags(
		FGameplayTagContainer(StatsTags::Ge_Stats_InCombat)
	);

	ASC->RemoveActiveEffectsWithGrantedTags(
		FGameplayTagContainer(StatsTags::Ge_Stats_CanAttack)
	);
}
