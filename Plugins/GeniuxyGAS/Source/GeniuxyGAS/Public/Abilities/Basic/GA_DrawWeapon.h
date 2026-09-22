// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "GA_DrawWeapon.generated.h"

class UAbilityTask_WaitInputPress;
/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGA_DrawWeapon : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_DrawWeapon();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool PreLoadMontage() override;

private:
	UFUNCTION()
	void HandleInputPress(float TimeWaited);
	UFUNCTION()
	void OnSheatheSwordEnd();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DrawSwordMontageTag;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag SheatheSwordMontageTag;
	
	UPROPERTY()
	UAnimMontage* DrawSwordMontage = nullptr;
	
	UPROPERTY()
	UAnimMontage* SheatheSwordMontage = nullptr;

	UPROPERTY()
	UAbilityTask_WaitInputPress* WaitInputPress = nullptr;
};
