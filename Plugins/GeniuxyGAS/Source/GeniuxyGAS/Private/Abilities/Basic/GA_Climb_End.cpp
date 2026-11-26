// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Climb_End.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Tags/AbilityTags.h"
#include "Tags/EventTags.h"
#include "Tags/StatsTags.h"


UGA_Climb_End::UGA_Climb_End()
{
	AbilityTags.AddTag(AbilityTags::Ge_Ability_Climb_End);
	AbilityTags.AddTag(AbilityTags::Ge_Ability_Group_Climb);
	ActivationRequiredTags.AddTag(StatsTags::Ge_Stats_Climbing);

	CancelAbilitiesWithTag.AddTag(AbilityTags::Ge_Ability_Group_Ground);
	BlockAbilitiesWithTag.AddTag(AbilityTags::Ge_Ability_Group_Ground);

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	ClimbEndEventTag = EventTags::Ge_Event_Climb_End;
}

void UGA_Climb_End::OnActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !GetOwnerASC())
	{
		K2_EndAbility();
		return;
	}
	
	if (K2_HasAuthority())
	{
		FGameplayEventData EventData;
		EventData.EventTag = ClimbEndEventTag;
		EventData.Instigator = Avatar;
		EventData.Target = Avatar;

		// 发给当前角色，由角色处理"销毁自己 + 生成新角色"
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			Avatar, ClimbEndEventTag, EventData
		);
	
		K2_EndAbility();
	}
}
