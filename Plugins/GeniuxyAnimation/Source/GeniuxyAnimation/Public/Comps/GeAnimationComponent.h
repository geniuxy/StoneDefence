// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GeniuxyAnimationType.h"
#include "Components/ActorComponent.h"
#include "GeAnimationComponent.generated.h"

class UCharacterAnimationSet;

UCLASS(ClassGroup=(Animation), meta=(BlueprintSpawnableComponent))
class GENIUXYANIMATION_API UGeAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGeAnimationComponent();

	UFUNCTION(BlueprintCallable)
	void SetAnimationSet(UCharacterAnimationSet* InAnimationSet);

	UFUNCTION(BlueprintCallable)
	bool PlayAction(FGameplayTag ActionTag);

private:
	bool PlayLoadedMontage(const FCharacterMontageEntry& Entry);

private:
	UPROPERTY()
	TObjectPtr<UCharacterAnimationSet> AnimationSet;
};
