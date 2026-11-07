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
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** 当前移动模式对应的最大速度（走 MaxWalkSpeed / MaxFlySpeed / MaxSwimSpeed） */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetMaxSpeed() const { return MaxSpeed; }

	/**
	 * 归一化速度：LocalVelocity / MaxSpeed，并夹到 [-1,1]。
	 * X=前后 Y=左右 Z=上下，混合空间三轴直接接它即可。
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetNormalizedVelocity() const { return NormalizedVelocity; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetNormalizedSpeed() const { return NormalizedVelocity.Size(); }

protected:
	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;

	/** 下面这些只在 CacheDerivedState（游戏线程）里写，工作线程只读 */
	float MaxSpeed = 0.f;
	FVector NormalizedVelocity = FVector::ZeroVector;

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

	/**********************************************************************/
	/*                                Lean                                */
	/**********************************************************************/
public:
	/** 2D Lean BlendSpace 的两个轴：X = 左右，Y = 上下，范围 [-1,1] */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Lean", meta = (BlueprintThreadSafe))
	float LeanX = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Lean", meta = (BlueprintThreadSafe))
	float LeanY = 0.f;

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetLeanX() const { return LeanX; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetLeanY() const { return LeanY; }

	/** 倾斜到满幅度所需速度（cm/s）。低于它时倾斜按比例衰减（悬停=0） */
	UPROPERTY(EditAnywhere, Category = "Animation|Lean", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float LeanFullSpeed = 600.f;

	/** 平滑速度，对应参考 ABP 里 FInterpTo 的 InterpSpeed */
	UPROPERTY(EditAnywhere, Category = "Animation|Lean", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float LeanInterpSpeed = 5.f;

	/** 死区：目标值低于它就当 0，避免悬停时速度抖动带出细微倾斜 */
	UPROPERTY(EditAnywhere, Category = "Animation|Lean", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LeanDeadZone = 0.02f;

protected:
	/** 游戏线程：把本地空间速度拆成左右/上下两个倾斜分量并平滑，写入 Lean / LeanX / LeanY */
	void UpdateLean(float DeltaSeconds);

	/**********************************************************************/
	/*                               LookAt                               */
	/**********************************************************************/
public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation|LookAt", meta = (BlueprintThreadSafe))
	FVector LookAtLocation = FVector::ZeroVector;

	/** 和 LookAtLocation 同值。给 C++ 用，风格与本类其它缓存量的 Get*() 保持一致 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetLookAtLocation() const { return LookAtLocation; }

	/**
	 * 目标点到眼睛的距离（cm，默认 100m）。
	 * 只用来把「方向」变成「点」，所以取大一点最好：眼睛和头骨之间那点位置差，
	 * 换算成角度会变成几度的抬头/低头误差，距离越远影响越小。
	 */
	UPROPERTY(EditAnywhere, Category = "Animation|LookAt", meta = (ClampMin = "100.0", UIMin = "100.0"))
	float LookAtDistance = 10000.f;

protected:
	/**
	 * 游戏线程：把瞄准方向折算成世界空间目标点，写入 LookAtLocation。
	 *
	 * 想改成「锁定某个敌人」这类外部目标时，在这里改就行：
	 * 拿 Target 的 GetActorLocation() 直接赋给 LookAtLocation 即可，
	 * 不要再额外加一个同名的变量，Look At 节点只认这一个。
	 */
	void UpdateLookAtLocation();
};
