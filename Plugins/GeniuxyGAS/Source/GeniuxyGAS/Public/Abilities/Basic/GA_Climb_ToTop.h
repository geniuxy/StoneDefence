// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "Tags/MontageTags.h"
#include "GA_Climb_ToTop.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGA_Climb_ToTop : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Climb_ToTop();

	virtual bool PreLoadMontage() override;

	virtual void OnActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Climb|Montage", meta = (Categories = "Ge.Montage"))
	FGameplayTag ClimbToTopMontageTag = MontageTags::Ge_Montage_Climb_ToTop;

	UPROPERTY()
	UAnimMontage* ClimbToTopMontage = nullptr;

	UFUNCTION()
	void HandleMontageEnded();
};
