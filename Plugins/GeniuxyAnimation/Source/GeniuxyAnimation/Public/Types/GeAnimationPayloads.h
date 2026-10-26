// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeAnimationPayloads.generated.h"

/**
 * ============================================================================
 *  GeniuxyAnimation 的 GameplayEvent 载荷 / 包装 UObject 集中放这里。
 * ============================================================================
 */

/**
 * UAN_DrawWeapon（AnimNotify）-> 角色 的 GameplayEvent 载荷。
 *
 * 收/拔武器事件里要带"武器挂在哪个 Socket 上"，而 FName 塞不进
 * FGameplayEventData 的普通字段（可用的只有 OptionalObject / Instigator 这类），
 * 所以包一层 UObject 当数据袋子。
 */
UCLASS(Blueprintable)
class GENIUXYANIMATION_API USocketNameWrapper : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FName SocketName;
};
