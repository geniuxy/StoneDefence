// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Sprint.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "Effects/Modifier/GE_Ground_Sprint_Speed.h"
#include "Tags/AbilityTags.h"
#include "Tags/StatsTags.h"

UGA_Sprint::UGA_Sprint()
{
	AbilityTags.AddTag(AbilityTags::Ge_Ability_Group_Ground);
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Sprinting);
	ActivationBlockedTags.AddTag(StatsTags::Ge_Stats_Walking);
	
	IncreaseMoveSpeedEffect = UGE_Ground_Sprint_Speed::StaticClass();
}

void UGA_Sprint::OnActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !IncreaseMoveSpeedEffect)
	{
		K2_EndAbility();
		return;
	}

	if (K2_HasAuthority())
	{
		IncreaseMoveSpeedEffectHandle = BP_ApplyGameplayEffectToOwner(
			IncreaseMoveSpeedEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
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

void UGA_Sprint::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (GetOwnerASC() && IncreaseMoveSpeedEffectHandle.IsValid())
	{
		GetOwnerASC()->RemoveActiveGameplayEffect(IncreaseMoveSpeedEffectHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Sprint::HandleInputPress(float TimeWaited)
{
	K2_EndAbility();
}

void UGA_Sprint::HandleInputRelease(float TimeHeld)
{
	K2_EndAbility();
}
