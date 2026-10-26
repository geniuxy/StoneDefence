// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeAnimInstanceLocoBase.h"
#include "GeAnimInstanceLocoSwim.generated.h"

/**
 * 游泳层实例（对应 ABP 里的 SwimLocomotion 层）。
 * 和飞行层一样是独立 UAnimInstance，自己从 Pawn / 组件取数据。
 */
UCLASS()
class GENIUXYANIMATION_API UGeAnimInstanceLocoSwim : public UGeAnimInstanceLocoBase
{
	GENERATED_BODY()

public:
	/** 归一化游泳速度，直接接混合空间 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetSwimSpeed() const { return GetNormalizedVelocity(); }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsDiving() const { return bIsDiving; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsSurfaceSwimming() const { return !bIsDiving; }

protected:
	virtual void CacheDerivedState() override;

	/** 只在 CacheDerivedState（游戏线程）里写 */
	bool bIsDiving = false;
};
