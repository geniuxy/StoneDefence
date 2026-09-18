// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeniuxyGASType.generated.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None UMETA(DisplayName="None"),
	BasicAttack UMETA(DisplayName="Basic Attack"),
	Aim UMETA(DisplayName="Aim"),
	AbilityOne UMETA(DisplayName="Ability One"),
	AbilityTwo UMETA(DisplayName="Ability Two"),
	AbilityThree UMETA(DisplayName="Ability Three"),
	AbilityFour UMETA(DisplayName="Ability Four"),
	AbilityFive UMETA(DisplayName="Ability Five"),
	AbilitySix UMETA(DisplayName="Ability Six"),
	Confirm UMETA(DisplayName="Confirm"),
	Cancel UMETA(DisplayName="Cancel"),
	Sprint UMETA(DisplayName="Sprint"),
	LockTarget UMETA(DisplayName="LockTarget"),
	SummonMount UMETA(DisplayName="SummonMount")
};

USTRUCT(BlueprintType)
struct FHeroBaseStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> HeroClass;

	UPROPERTY(EditAnywhere)
	float Strength = 0.f;

	UPROPERTY(EditAnywhere)
	float Intelligence = 0.f;

	UPROPERTY(EditAnywhere)
	float StrengthGrowthRate = 0.f;

	UPROPERTY(EditAnywhere)
	float IntelligenceGrowthRate = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseMaxHealth = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseMaxEnergy = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseAttackDamage = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseArmor = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseMoveSpeed = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseMoveAcceleration = 2048.f;

	UPROPERTY(EditAnywhere)
	float BaseWeightLimit = 200.f;

	UPROPERTY(EditAnywhere)
	float BaseGold = 300.f;
};
