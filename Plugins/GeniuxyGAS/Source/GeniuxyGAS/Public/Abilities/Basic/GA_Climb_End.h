// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "GA_Climb_End.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGA_Climb_End : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Climb_End();

	virtual void OnActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTag ClimbEndEventTag;
};
