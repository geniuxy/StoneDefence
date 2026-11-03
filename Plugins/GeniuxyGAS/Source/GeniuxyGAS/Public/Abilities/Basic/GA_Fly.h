// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "GA_Fly.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGA_Fly : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Fly();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UFUNCTION()
	void HandleInputPress(float TimeWaited);
};
