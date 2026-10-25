// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GeFlyComponent.generated.h"

class UCharacterMovementComponent;
class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GENIUXYANIMATION_API UGeFlyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGeFlyComponent();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	ACharacter* OwnerCharacter;
	UPROPERTY()
	UAbilitySystemComponent* OwnerASC;
	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;
	
	void OwnerFlyTagUpdated(const FGameplayTag Tag, int32 NewCount);
};
