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
 * 特别注意：layer 实例（UGeAnimInstanceLocoBase，即 ABP_GeLoco 及其子类的实例）是
 * **独立的 UAnimInstance 对象**，看不到主实例的成员变量。所以凡是两个实例都要用的数据，
 * 统一从 UGeLocomotionComponent 上取，不要试图跨实例访问。
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
	float Speed = 0.f;
	float Speed2D = 0.f;
	FVector WorldVelocity = FVector::ZeroVector;
	FVector LocalVelocity = FVector::ZeroVector;

	EGeLocomotionMode LocomotionMode = EGeLocomotionMode::Ground;
	bool bInCombat = false;
	bool bIsInAir = false;

	/**********************************************************************/
	/*                           Yaw Rotation                             */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawSpeed() const { return YawSpeed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSmoothedYawSpeed() const { return SmoothedYawSpeed; }

protected:
	FRotator BodyPrevRot;
	float YawSpeed;
	float SmoothedYawSpeed;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float YawSpeedSmoothLerpSpeed = 1.f;
	UPROPERTY(EditAnywhere, Category = "Animation")
	float YawSpeedLerpToZeroSpeed = 30.f;

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

	/**
	 * 按当前 Mesh 的骨架重新检测有没有 IK 脚骨骼，并覆盖 bHasIKBone。
	 *
	 * 为什么必须自动检测、而不能靠 ABP 的 Class Defaults 手填：
	 * 主实例（ABP_SdHeroBase_C）和层实例（ABP_GeLoco_Cleric_C / ABP_GeLoco_Tubaki_C）
	 * 是**两个独立的 UAnimInstance 对象**，各自有一份 bHasIKBone。
	 * FootIK 的 ControlRig 在主 ABP 里，读的是**主实例**那一份；
	 * 在层 ABP 上把 bHasIKBone 改成 false 只改了层实例的副本，没有任何人读它。
	 * 而主 ABP 只有一份，它的类默认值无法按武将区分 —— 所以只能运行时从骨架里查。
	 */
	UFUNCTION(BlueprintCallable, Category = "AnimAttrubute|FootIK")
	void DetectIKBoneSupport();

protected:
	virtual void InitFootIKId(ACharacter* OwnerCharacter);
	void CalcFootIKOffset();

	/**
	 * 兜底的脚部 IK 开关，**只在拿不到 UGeLocomotionComponent 时生效**（例如预览 Actor）。
	 *
	 * 角色请配 UGeAnimationSetup（挂在 UGeLocomotionComponent 上），那条数据线才是对的：
	 *   UGeLocomotionComponent::IsFootIKEnabled()
	 * 在 ABP 的 Class Defaults 里改这个值对角色无效（主实例和 layer 实例各有一份类默认值）。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	bool bCanFootIK = false;

	/**
	 * 骨架是否带 IK 脚骨骼。
	 * bAutoDetectIKBone 开启时这个值会在初始化时被 DetectIKBoneSupport 覆盖，
	 * 在 ABP 的 Class Defaults 里手填是无效的（见 DetectIKBoneSupport 的注释）。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	bool bHasIKBone = false;

	/** 是否按骨架自动检测 bHasIKBone。只有骨架命名不规范、需要硬编码时才关掉 */
	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	bool bAutoDetectIKBone = true;

	/** 检测用：IK 左脚骨骼名（两具骨架都有 foot_l，但只有带 IK 的才有 ik_foot_l） */
	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	FName IKFootLeftBoneName = TEXT("ik_foot_l");

	/** 检测用：IK 右脚骨骼名 */
	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	FName IKFootRightBoneName = TEXT("ik_foot_r");

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
