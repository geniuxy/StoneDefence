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

	// 拔剑 = 进入战斗姿态。真正给 ASC 贴 InCombat / CanAttack 的是
	// AM_DrawSword 里的 AN_ApplyGameplayEffect，这里只记录逻辑状态。
	bCombatStance = true;
	bStanceCleared = false;

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayDrawSwordMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DrawSwordMontage);
		// 只绑 OnCancelled（montage 根本没播出来）。
		// 千万不要绑 OnInterrupted / OnBlendOut / OnCompleted：
		// UAbilitySystemComponent::PlayMontage 内部走的是 Montage_Play(..., bStopAllMontages = true)，
		// 收剑时播 montage 会把这段拔剑 montage 以 bInterrupted=true 顶掉。
		PlayDrawSwordMontage->OnCancelled.AddDynamic(this, &ThisClass::OnDrawSwordCancelled);
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
	// 兜底：只要能力在"战斗姿态"下结束（不管是正常收剑、被取消还是被打断），
	// 都保证把 InCombat / CanAttack 收掉，避免 tag 永久残留。
	if (bCombatStance)
	{
		ClearStanceEffects();
	}

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
	if (!bCombatStance)
	{
		// 还没进入战斗姿态（拔剑流程还没走完），这次按键不算"收剑"，继续等下一次。
		ArmInputWait();
		return;
	}

	// 收剑期间不要再 ArmInputWait()：
	// 否则下一次按键会在收剑 montage 还没播完时再 Montage_Play 一次，
	// 而 ASC::PlayMontage 默认 bStopAllMontages=true，会把自己这条收剑 montage 打断。
	WaitInputPress = nullptr;

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

void UGA_DrawWeapon::OnDrawSwordCancelled()
{
	// 拔剑 montage 播不出来（资源缺失 / 没有 AnimInstance 等），直接结束，避免能力卡住。
	K2_EndAbility();
}

void UGA_DrawWeapon::ClearStanceEffects()
{
	// OnSheatheSwordEnd 和 EndAbility 都可能进来，做成幂等的。
	bCombatStance = false;
	if (bStanceCleared)
	{
		return;
	}
	bStanceCleared = true;

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
