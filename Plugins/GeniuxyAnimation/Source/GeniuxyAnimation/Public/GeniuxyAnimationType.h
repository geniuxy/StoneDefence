#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GeniuxyAnimationType.generated.h"

USTRUCT(BlueprintType)
struct FCharacterMontageEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ActionTag;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly)
	FName StartSection;

	UPROPERTY(EditDefaultsOnly)
	float PlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredTags;
};

/**
 * 移动模式（Locomotion Mode）。
 *
 * 和「战斗状态」是两件正交的事：可以边飞边战斗、边游边战斗，
 * 所以不要再把它们合并进同一个枚举（旧做法会导致 Fly tag 一掉就把战斗状态清成 NORMAL）。
 *
 * 这个枚举驱动主 ABP 里的 BlendListByEnum，值顺序即引脚顺序，改动会打断已连好的引脚，
 * 因此只在末尾追加，不要插在中间。
 */
UENUM(BlueprintType)
enum class EGeLocomotionMode : uint8
{
	Ground UMETA(DisplayName="Ground"),
	Fly UMETA(DisplayName="Fly"),
	Swim UMETA(DisplayName="Swim"),
	Climb UMETA(DisplayName="Climb")
};
