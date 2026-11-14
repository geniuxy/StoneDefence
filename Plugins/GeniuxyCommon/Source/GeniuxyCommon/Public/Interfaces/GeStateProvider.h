// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GeStateProvider.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UGeStateProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 「状态」提供者 —— 动画侧与 GAS 侧之间的依赖倒置接口。
 */
class GENIUXYCOMMON_API IGeStateProvider
{
	GENERATED_BODY()

public:
	/** 当前是否在战斗中（实现方返回缓存值，不做任何查找） */
	virtual bool IsInCombat() const = 0;
	virtual bool IsGroundWalking() const = 0;
	virtual bool IsGroundSprinting() const = 0;
};
