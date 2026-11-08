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
	SummonMount UMETA(DisplayName="SummonMount"),
	Tab UMETA(DisplayName="Tab"),
	Fly UMETA(DisplayName="Fly"),
	Ctrl UMETA(DisplayName="Ctrl"),
	Dodge UMETA(DisplayName="Dodge"),
};

/**
 * 四向闪避的方向。方向是相对「视角」算的，不是相对角色。
 *
 * - 地面闪避：前 / 后 / 左 / 右（只看水平面）
 * - 飞行闪避：上 / 下 / 左 / 右（Forward 就是飞行蒙太奇里的 U，Backward 是 D）
 */
UENUM(BlueprintType)
enum class EDodgeDirection : uint8
{
	/** 地面：向前；飞行：向上（U） */
	Forward UMETA(DisplayName = "Forward / Up"),
	/** 地面：向后；飞行：向下（D） */
	Backward UMETA(DisplayName = "Backward / Down"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
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
	float BaseFlySpeed = 600.f;

	UPROPERTY(EditAnywhere)
	float BaseMoveAcceleration = 2048.f;

	UPROPERTY(EditAnywhere)
	float BaseWeightLimit = 200.f;

	UPROPERTY(EditAnywhere)
	float BaseGold = 300.f;
};
