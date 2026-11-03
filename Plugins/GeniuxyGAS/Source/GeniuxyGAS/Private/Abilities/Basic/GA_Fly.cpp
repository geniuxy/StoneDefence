// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Fly.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Tags/StatsTags.h"

UGA_Fly::UGA_Fly()
{
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Fly);
}

void UGA_Fly::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_Fly::HandleInputPress(float TimeWaited)
{
	K2_EndAbility();
}
