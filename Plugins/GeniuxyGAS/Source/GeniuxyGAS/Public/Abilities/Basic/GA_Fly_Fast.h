// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "GA_Fly_Fast.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGA_Fly_Fast : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Fly_Fast();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UFUNCTION()
	void HandleInputRelease(float TimeWaited);
};
