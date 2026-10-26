// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeAnimInstanceBase.h"
#include "GeAnimInstanceLocoBase.generated.h"

class UCharacterMovementComponent;

/**
 * 所有「移动层」实例的公共基类。
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
	/*                                Fly                                 */
	/**********************************************************************/
public:
	/** 归一化飞行速度。和 GetNormalizedVelocity() 完全等价，只是飞行层里读着顺手 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetFlySpeed() const { return NormalizedVelocity; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsAscending() const { return bIsAscending; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsDescending() const { return bIsDescending; }

protected:
	bool bIsAscending = false;
	bool bIsDescending = false;

	/**********************************************************************/
	/*                                Swim                                */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetSwimSpeed() const { return NormalizedVelocity; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsDiving() const { return bIsDiving; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsSurfaceSwimming() const { return !bIsDiving; }

protected:
	bool bIsDiving = false;

	/**********************************************************************/
	/*                                Climb                               */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetClimbSpeed() const { return NormalizedVelocity; }

	/** 只取水平分量，给「四向攀爬」这种只需要左右两轴的混合空间用 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector2D GetClimbSpeed2D() const
	{
		return FVector2D(NormalizedVelocity.X, NormalizedVelocity.Y);
	}

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsClimbUp() const { return bIsClimbUp; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsClimbDown() const { return bIsClimbDown; }

protected:
	bool bIsClimbUp = false;
	bool bIsClimbDown = false;

protected:
	virtual void CacheDerivedState() override;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;

	/** 下面这些只在 CacheDerivedState（游戏线程）里写，工作线程只读 */
	float MaxSpeed = 0.f;
	FVector NormalizedVelocity = FVector::ZeroVector;
};
