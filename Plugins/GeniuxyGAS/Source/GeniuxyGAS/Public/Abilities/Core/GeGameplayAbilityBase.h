// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GeGameplayAbilityBase.generated.h"

class UGeAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGeGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	UGeAbilitySystemComponent* GetOwnerASC();

	UPROPERTY()
	UGeAbilitySystemComponent* OwnerASC;

	TSoftObjectPtr<UAnimMontage> GetMontageByTag(FGameplayTag InTag);

	virtual bool PreLoadMontage() { return true; }
};
