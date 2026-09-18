// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_DrawWeapon.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Tags/StatsTags.h"

UGA_DrawWeapon::UGA_DrawWeapon()
{
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_EquippingSword);
	// ActivationRequiredTags.AddTag(StatsTags::Ge_Stats_EquippingSword);
}

void UGA_DrawWeapon::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !GetOwnerASC())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayDrawSwordMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DrawSwordMontage);
		// PlayDrawSwordMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		// PlayDrawSwordMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		// PlayDrawSwordMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		// PlayDrawSwordMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayDrawSwordMontage->ReadyForActivation();
	}

	WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_DrawWeapon::HandleInputPress(float TimeWaited)
{
	if (!OwnerASC->HasMatchingGameplayTag(StatsTags::Ge_Stats_InCombat))
	{
		// TODO: 这里直接return不行吗？
		WaitInputPress->EndTask();
		WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
		WaitInputPress->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
		WaitInputPress->ReadyForActivation();
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlaySheatheSwordMontage =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, SheatheSwordMontage);
	PlaySheatheSwordMontage->OnCompleted.AddDynamic(this, &ThisClass::OnSheatheSwordEnd);
	PlaySheatheSwordMontage->OnBlendOut.AddDynamic(this, &ThisClass::OnSheatheSwordEnd);
	PlaySheatheSwordMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlaySheatheSwordMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlaySheatheSwordMontage->ReadyForActivation();
}

void UGA_DrawWeapon::OnSheatheSwordEnd()
{
	OwnerASC->RemoveActiveEffectsWithGrantedTags(
		FGameplayTagContainer(StatsTags::Ge_Stats_InCombat)
	);

	OwnerASC->RemoveActiveEffectsWithGrantedTags(
		FGameplayTagContainer(StatsTags::Ge_Stats_CanAttack)
	);

	K2_EndAbility();
}
