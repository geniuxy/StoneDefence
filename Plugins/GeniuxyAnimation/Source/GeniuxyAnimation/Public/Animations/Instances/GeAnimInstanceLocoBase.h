// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeAnimInstanceBase.h"
#include "GeAnimInstanceLocoBase.generated.h"

class UBlendSpace;
class UCharacterMovementComponent;
class UGeLocomotionComponent;
class UGeLocomotionAnimSet;

/**
 * 所有「移动层」实例的公共基类。
 *
 * 飞行 / 游泳 / 攀爬 三层的输入长得几乎一样：
 *   一个 -1~1 的三轴速度 + 当前模式的最大速度
 * 所以统一在这里算，Fly / Swim / Climb 只负责「用哪个混合空间、有没有本模式特有的开关」。
 */
UCLASS()
class GENIUXYANIMATION_API UGeAnimInstanceLocoBase : public UGeAnimInstanceBase
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	/** 当前移动模式对应的最大速度（走 MaxWalkSpeed / MaxFlySpeed / MaxSwimSpeed） */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetMaxSpeed() const { return MaxSpeed; }

	/**
	 * 归一化速度：LocalVelocity / MaxSpeed，并夹到 [-1,1]。
	 * X=前后 Y=左右 Z=上下，混合空间三轴直接接它即可。
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetNormalizedVelocity() const { return NormalizedVelocity; }

	/**********************************************************************/
	/*                      BlendSpace 取值（给 ABP 引脚）                  */
	/**********************************************************************/
	/** 没配 GeLocomotionAnimSet 时返回 nullptr，ABP 里要判空 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UBlendSpace* GetFlyBlendSpace() const;

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UBlendSpace* GetSwimBlendSpace() const;

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UBlendSpace* GetClimbBlendSpace() const;

	/** 移动动画集合；没配就是 nullptr，ABP 里要判空 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE UGeLocomotionAnimSet* GetLocomotionAnimSet() const { return LocomotionAnimSet; }

protected:
	virtual void CacheDerivedState() override;

	/** 移动模式 + 战斗状态 + 移动动画集合的唯一数据源，缓存在这里 */
	UPROPERTY()
	UGeLocomotionComponent* LocomotionComp;

	/** 移动动画集合，从 LocomotionComp 上取，本模式层实例共用同一份 */
	UPROPERTY()
	UGeLocomotionAnimSet* LocomotionAnimSet;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;

	/** 只在 CacheDerivedState（游戏线程）里写 */
	float MaxSpeed = 0.f;
	FVector NormalizedVelocity = FVector::ZeroVector;
};
