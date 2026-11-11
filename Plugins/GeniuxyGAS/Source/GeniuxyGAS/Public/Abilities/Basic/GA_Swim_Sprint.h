// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "GA_Swim_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGA_Swim_Sprint : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Swim_Sprint();

	virtual void OnActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> IncreaseMoveSpeedEffect;

	FActiveGameplayEffectHandle IncreaseMoveSpeedEffectHandle;

	UFUNCTION()
	void HandleInputRelease(float TimeWaited);
};
