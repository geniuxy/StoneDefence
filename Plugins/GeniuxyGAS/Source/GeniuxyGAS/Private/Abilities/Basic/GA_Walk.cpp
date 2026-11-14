// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Walk.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "Effects/Modifier/GE_Ground_Walk_Speed.h"
#include "Tags/AbilityTags.h"
#include "Tags/StatsTags.h"

UGA_Walk::UGA_Walk()
{
	AbilityTags.AddTag(AbilityTags::Ge_Ability_Group_Ground);
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Walking);
	ActivationBlockedTags.AddTag(StatsTags::Ge_Stats_Sprinting);

	DecreaseMoveSpeedEffect = UGE_Ground_Walk_Speed::StaticClass();
}

void UGA_Walk::OnActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !DecreaseMoveSpeedEffect)
	{
		K2_EndAbility();
		return;
	}

	if (K2_HasAuthority())
	{
		DecreaseMoveSpeedEffectHandle = BP_ApplyGameplayEffectToOwner(
			DecreaseMoveSpeedEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		);
	}

	bool bToggle;
	// if (IsValid(UDkGameUserSettings::Get()))
	// {
	// 	bToggle = UDkGameUserSettings::Get()->IsToggleSprintMode();
	// }
	// else
	// {
	bToggle = bIsToggle;
	// }
	if (bToggle)
	{
		UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
		WaitInputPress->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
		WaitInputPress->ReadyForActivation();
	}
	else
	{
		UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
		WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::HandleInputRelease);
		WaitInputRelease->ReadyForActivation();
	}
}

void UGA_Walk::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (GetOwnerASC() && DecreaseMoveSpeedEffectHandle.IsValid())
	{
		GetOwnerASC()->RemoveActiveGameplayEffect(DecreaseMoveSpeedEffectHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Walk::HandleInputPress(float TimeWaited)
{
	K2_EndAbility();
}

void UGA_Walk::HandleInputRelease(float TimeHeld)
{
	K2_EndAbility();
}
