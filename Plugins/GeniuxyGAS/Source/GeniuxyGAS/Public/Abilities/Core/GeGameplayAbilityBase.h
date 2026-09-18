// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GeGameplayAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGeGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UAbilitySystemComponent* GetOwnerASC();

	UPROPERTY()
	UAbilitySystemComponent* OwnerASC;
};
