// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeniuxyGASType.generated.h"

class UGameplayAbility;

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
 * 一条「能力条目」：InputID + 能力类 + 等级。
 *
 * 为什么不继续用 TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>>：
 * TMap 的键天然唯一，一个 InputID 只能挂一个 GA；而实际需要的是
 * 「同一个键，在地面 / 飞行 / 游泳状态下触发不同的 GA」。
 * 改成数组后同一个 InputID 可以有多条，每条都 Give 成独立的 AbilitySpec。
 *
 * 为什么不需要自己写「按状态选一个」：
 * 引擎的 UAbilitySystemComponent::AbilityLocalInputPressed 本来就会遍历所有 InputID
 * 相同的 spec 逐个 TryActivateAbility，谁的门控过了谁激活。门控有三把现成的锁：
 * - ActivationRequiredTags / ActivationBlockedTags：按 ASC 上的 Tag 判断能不能激活；
 * - 别人的 BlockAbilitiesWithTag / CancelAbilitiesWithTag：模式能力存活期间禁掉/打断整组能力。
 */
USTRUCT(BlueprintType)
struct FGeAbilitySpecEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EAbilityInputID InputID = EAbilityInputID::None;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> AbilityClass;

	/** 0 级 = 可学习但未学会（对应旧的 Abilities），1 级 = 初始可用（对应旧的 BasicAbilities） */
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (ClampMin = "0"))
	int32 Level = 1;

	/**
	 * 纯编辑器注释，标明这条属于哪个状态（例：地面冲刺 / 飞行加速），不参与任何逻辑。
	 * 真正的门控写在 GA 自己的 AbilityTags / ActivationRequiredTags 上，
	 * 保持「一条能力的状态要求只有一个真源」。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FString EditorComment;
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
	float BaseSwimSpeed = 300.f;

	UPROPERTY(EditAnywhere)
	float BaseMoveAcceleration = 2048.f;

	UPROPERTY(EditAnywhere)
	float BaseWeightLimit = 200.f;

	UPROPERTY(EditAnywhere)
	float BaseGold = 300.f;
};
