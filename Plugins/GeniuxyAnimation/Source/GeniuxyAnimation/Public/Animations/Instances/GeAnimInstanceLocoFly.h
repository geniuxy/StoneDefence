// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeAnimInstanceLocoBase.h"
#include "GeAnimInstanceLocoFly.generated.h"

/**
 * 飞行层实例（对应 ABP 里的 FlyLocomotion 层）。
 *
 * 这个类是**独立的 UAnimInstance 对象**，和主实例不共享变量。
 * 它需要的一切都自己从 UGeLocomotionComponent / Pawn 上取，所以不会有跨实例的数据竞争。
 *
 * 老的 ABP_Template_Fly 里接 `GetFlySpeed().X / .Y` 的引脚不用改：
 * GetFlySpeed() 现在直接转发到基类的归一化速度。
 */
UCLASS()
class GENIUXYANIMATION_API UGeAnimInstanceLocoFly : public UGeAnimInstanceLocoBase
{
	GENERATED_BODY()

public:
	/** 归一化飞行速度，X=前后 Y=左右 Z=上下，范围 [-1,1]。直接接混合空间三轴 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetFlySpeed() const { return GetNormalizedVelocity(); }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsAscending() const { return bIsAscending; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsDescending() const { return bIsDescending; }

protected:
	virtual void CacheDerivedState() override;

	/** 只在 CacheDerivedState（游戏线程）里写 */
	bool bIsAscending = false;
	bool bIsDescending = false;
};
