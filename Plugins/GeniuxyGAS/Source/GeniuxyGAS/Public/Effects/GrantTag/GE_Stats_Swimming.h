// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/GrantTag/Core/GE_GrantTags.h"
#include "GE_Stats_Swimming.generated.h"

/**
 * 游泳状态 GE：给目标挂上 Ge.Stats.Swim。
 *
 * 挂上 / 摘掉由 ASdWaterVolume 负责（入水 Apply、出水按 FActiveGameplayEffectHandle Remove）。
 * 挂上之后 UGeLocomotionComponent 收到 tag 事件，会把移动模式切到 Swim
 * （ApplyMovementMode -> SetMovementMode(MOVE_Swimming)），动画层和速度随之切换。
 */
UCLASS()
class GENIUXYGAS_API UGE_Stats_Swimming : public UGE_GrantTags
{
	GENERATED_BODY()

public:
	UGE_Stats_Swimming();
};
