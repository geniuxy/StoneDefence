// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Swim_Sprint.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "Effects/Modifier/GE_Swim_Sprint_Speed.h"
#include "Tags/AbilityTags.h"
#include "Tags/StatsTags.h"

UGA_Swim_Sprint::UGA_Swim_Sprint()
{
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Swim_Fast);
	ActivationRequiredTags.AddTag(StatsTags::Ge_Stats_Swim);

	AbilityTags.AddTag(AbilityTags::Ge_Ability_Group_Swim);

	IncreaseMoveSpeedEffect = UGE_Swim_Sprint_Speed::StaticClass();
}

void UGA_Swim_Sprint::OnActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!IncreaseMoveSpeedEffect)
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

	UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::HandleInputRelease);
	WaitInputRelease->ReadyForActivation();
}

void UGA_Swim_Sprint::EndAbility(
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

void UGA_Swim_Sprint::HandleInputRelease(float TimeWaited)
{
	K2_EndAbility();
}
