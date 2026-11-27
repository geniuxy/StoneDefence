// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Climb_ToTop.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Tags/AbilityTags.h"
#include "Tags/EventTags.h"
#include "Tags/StatsTags.h"

UGA_Climb_ToTop::UGA_Climb_ToTop()
{
	AbilityTags.AddTag(AbilityTags::Ge_Ability_Climb_ToTop);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = EventTags::Ge_Event_Climb_ToTop;
	AbilityTriggers.Add(TriggerData);

	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_ClimbingToTop);

	ActivationRequiredTags.AddTag(StatsTags::Ge_Stats_Climbing); 

	ActivationBlockedTags.AddTag(StatsTags::Ge_Stats_Dead);

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

bool UGA_Climb_ToTop::PreLoadMontage()
{
	ClimbToTopMontage = LoadMontageByTag(ClimbToTopMontageTag);

	if (!ClimbToTopMontage)
	{
		return false;
	}

	return true;
}

void UGA_Climb_ToTop::OnActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ClimbToTopMontage);

	if (!MontageTask)
	{
		K2_EndAbility();
		return;
	}

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::HandleMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::HandleMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::HandleMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::HandleMontageEnded);
	MontageTask->ReadyForActivation();
}

void UGA_Climb_ToTop::HandleMontageEnded()
{
	if (IsActive())
	{
		K2_EndAbility();
	}
}
