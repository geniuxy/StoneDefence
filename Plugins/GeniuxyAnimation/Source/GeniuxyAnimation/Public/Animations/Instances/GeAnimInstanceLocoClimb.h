// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeAnimInstanceLocoBase.h"
#include "GeAnimInstanceLocoClimb.generated.h"

/**
 * 攀爬层实例（对应 ABP 里的 ClimbLocomotion 层）。
 * 和飞行/游泳一样是独立 UAnimInstance，自己从 Pawn / 组件取数据。
 */
UCLASS()
class GENIUXYANIMATION_API UGeAnimInstanceLocoClimb : public UGeAnimInstanceLocoBase
{
	GENERATED_BODY()

public:
	/** 归一化攀爬速度，X=前后 Y=左右 Z=上下，直接接混合空间 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetClimbSpeed() const { return GetNormalizedVelocity(); }

	/** 只取水平分量，给「四向攀爬」这种只需要左右两轴的混合空间用 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector2D GetClimbSpeed2D() const
	{
		return FVector2D(GetNormalizedVelocity().X, GetNormalizedVelocity().Y);
	}

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsClimbUp() const { return bIsClimbUp; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsClimbDown() const { return bIsClimbDown; }

protected:
	virtual void CacheDerivedState() override;

	/** 只在 CacheDerivedState（游戏线程）里写 */
	bool bIsClimbUp = false;
	bool bIsClimbDown = false;
};
