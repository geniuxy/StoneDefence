// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Fly_Fast.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Tags/StatsTags.h"

UGA_Fly_Fast::UGA_Fly_Fast()
{
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Fly_Fast);
	ActivationRequiredTags.AddTag(StatsTags::Ge_Stats_Fly);
}

void UGA_Fly_Fast::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::HandleInputRelease);
	WaitInputRelease->ReadyForActivation();
}

void UGA_Fly_Fast::HandleInputRelease(float TimeWaited)
{
	K2_EndAbility();
}
