// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeniuxyAnimationType.h"
#include "Animation/AnimInstance.h"
#include "GeAnimInstanceBase.generated.h"

class UCharacterMovementComponent;
class USkeletalMeshComponent;

/**
 * 所有 AnimInstance 的公共基类（主实例和 layer 实例都用它）。
 *
 * 线程模型（整个插件的统一约定，务必遵守）：
 *   NativeUpdateAnimation            —— 游戏线程。唯一允许读 Actor / MovementComponent / 组件的地方。
 *   NativeThreadSafeUpdateAnimation  —— 动画工作线程。只能读下面这些已经缓存到成员里的标量。
 *   Get*() / BlueprintThreadSafe     —— 动画工作线程。只读缓存，不读 Actor。
 *
 * 特别注意：layer 实例（UGeAnimInstanceLocoFly / Swim / Climb）是**独立的 UAnimInstance 对象**，
 * 看不到主实例的成员变量。所以凡是两个实例都要用的数据，统一从 UGeLocomotionComponent 上取，
 * 不要试图跨实例访问。
 */
UCLASS()
class GENIUXYANIMATION_API UGeAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	USkeletalMeshComponent* OwnerSkeletalMeshComp;

	UPROPERTY()
	APawn* OwnerPawn;

	/** 每帧从 OwnerPawn 缓存一次，供派生类在自己的 NativeUpdateAnimation 里复用 */
	virtual void CacheOwnerState();

	/** 派生类可以重载它来追加自己的缓存逻辑（跑在游戏线程） */
	virtual void CacheDerivedState();

	/**********************************************************************/
	/*                             Locomotion                             */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

	/** 水平速度（忽略 Z），地面/游泳混合空间通常用这个 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetSpeed2D() const { return Speed2D; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetWorldVelocity() const { return WorldVelocity; }

	/** 角色本地空间的速度，X=前 Y=右 Z=上。混合空间直接接这个比接世界速度省事 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetLocalVelocity() const { return LocalVelocity; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE EGeLocomotionMode GetLocomotionMode() const { return LocomotionMode; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsGrounded() const { return LocomotionMode == EGeLocomotionMode::Ground; }

	/** 兼容旧 ABP 引脚：等价于 LocomotionMode == Fly */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsFlying() const { return LocomotionMode == EGeLocomotionMode::Fly; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsSwimming() const { return LocomotionMode == EGeLocomotionMode::Swim; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsClimbing() const { return LocomotionMode == EGeLocomotionMode::Climb; }

	/** 战斗状态。和移动模式正交，可以边飞边打 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsInCombat() const { return bInCombat; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsInAir() const { return bIsInAir; }

protected:
	/** 下面这些只在 NativeUpdateAnimation（游戏线程）里写 */
	float Speed = 0.f;
	float Speed2D = 0.f;
	FVector WorldVelocity = FVector::ZeroVector;
	FVector LocalVelocity = FVector::ZeroVector;

	EGeLocomotionMode LocomotionMode = EGeLocomotionMode::Ground;
	bool bInCombat = false;
	bool bIsInAir = false;

	/**********************************************************************/
	/*                              Foot IK                               */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetLeftOffset() const { return LeftOffset; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetRightOffset() const { return RightOffset; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetButtZOffset() const { return ButtZOffset; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsFootIK() const { return bFootIK; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool HasIKBone() const { return bHasIKBone; }

protected:
	virtual void InitFootIKId(ACharacter* OwnerCharacter);
	void CalcFootIKOffset();

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	bool bCanFootIK = false;

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	bool bHasIKBone = false;

	bool bFootIK = false;

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	FName LeftBoneName = TEXT("foot_l");

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	FName RightBoneName = TEXT("foot_r");

	float LeftOffset = 0.f;
	float RightOffset = 0.f;
	float ButtZOffset = 0.f;
	int32 FootIKId = INDEX_NONE;
	TArray<FName> BoneNames;

public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetFootIKOffset(const FName &InBoneName);
};
