// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeniuxyAnimationType.h"
#include "Animation/AnimInstance.h"
#include "GeAnimInstanceBase.generated.h"

class UCharacterMovementComponent;
class USkeletalMeshComponent;
class UAbilitySystemComponent;

/**
 * 所有 AnimInstance 的公共基类（主实例和 layer 实例都用它）。
 *
 * 职责范围：**和移动模式无关**的通用动画数据 —— 速度、朝向角速度、脚部 IK、战斗状态。
 * 凡是需要知道「现在是什么移动模式」（Ground/Fly/Swim/Climb）的数据与 getter，
 * 一律在 UGeAnimInstanceLocoBase（layer 实例那一条线）上，本类不碰 UGeLocomotionComponent
 * 的模式，也不会缓存模式字段 —— 这样预览实例、任何 ABP 都能直接继承本类。
 *
 * 线程模型（整个插件的统一约定，务必遵守）：
 *   NativeUpdateAnimation            —— 游戏线程。唯一允许读 Actor / MovementComponent / 组件的地方。
 *   NativeThreadSafeUpdateAnimation  —— 动画工作线程。只能读下面这些已经缓存到成员里的标量。
 *   Get*() / BlueprintThreadSafe     —— 动画工作线程。只读缓存，不读 Actor。
 *
 * 特别注意：layer 实例（UGeAnimInstanceLocoBase，即 ABP_GeLoco 及其子类的实例）是
 * **独立的 UAnimInstance 对象**，看不到主实例的成员变量。所以凡是两个实例都要用的数据，
 * 统一从「拥有者身上的组件」取（移动模式/脚 IK → UGeLocomotionComponent，
 * 战斗状态 → GAS），不要试图跨实例访问。
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

	/**
	 * 拥有者的 ASC（弱引用），只用来读战斗状态这一类 GAS 数据。
	 * 预览实例 / 没有 GAS 的 Pawn 上会是空的，所以每处使用都要判空。
	 */
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComp;

	/**
	 * 上一次尝试解析 ASC 时的 Pawn。
	 * 同一份动画数据每帧都会读战斗状态，但「找 ASC」只需要一次：
	 * 找不到就说明这个 Pawn 根本没有 GAS（预览 Actor 等），不必每帧再遍历一遍组件。
	 */
	TWeakObjectPtr<APawn> ResolvedForPawn;

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

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed != 0.f; }

	/** 角色本地空间的速度，X=前 Y=右 Z=上。混合空间直接接这个比接世界速度省事 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetLocalVelocity() const { return LocalVelocity; }
	
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsInAir() const { return bIsInAir; }

	/**
	 * 当前是否有移动输入（按着 WASD / AI 正在往路径点走）。
	 *
	 * 注意：这是「输入」不是「有没有在动」——撞墙、被定身时按着键也可能是 true，
	 * 想判断真的在位移请用 GetSpeed()。
	 * 也只在本地控制端（或服务器上的 AI）有意义：MovementInput 不复制，
	 * 模拟端读到的永远是 false。
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetHasMovementInput() const { return bHasMovementInput; }

protected:
	float Speed = 0.f;
	float Speed2D = 0.f;
	FVector WorldVelocity = FVector::ZeroVector;
	FVector LocalVelocity = FVector::ZeroVector;

	bool bIsInAir = false;
	bool bHasMovementInput = false;

	/**********************************************************************/
	/*                              Combat                                */
	/**********************************************************************/
public:
	/**
	 * 战斗状态。和移动模式正交，可以边飞边打。
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsInCombat() const { return bInCombat; }

protected:
	bool bInCombat = false;

	bool ReadInCombatFromGAS();

	/**********************************************************************/
	/*                        Yaw/Pitch Rotation                          */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawSpeed() const { return YawSpeed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSmoothedYawSpeed() const { return SmoothedYawSpeed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetNormalizedYawSpeed() const { return NormalizedYawSpeed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetPitchSpeed() const { return PitchSpeed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSmoothedPitchSpeed() const { return SmoothedPitchSpeed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetNormalizedPitchSpeed() const { return NormalizedPitchSpeed; }

protected:
	/**
	 * 取「Pitch 角速度」用的采样源。
	 */
	FRotator GetPitchSourceRotation() const;

	/** 重新对齐上一帧采样、并把所有角速度归零。初始化时调用（否则首帧 Yaw 会出现巨大尖峰） */
	void ResetRotationSpeedTracking();

	/** Yaw 用 Actor 朝向（地面移动时只有 Yaw 会变），Pitch 用 GetPitchSourceRotation() */
	FRotator BodyPrevRot = FRotator::ZeroRotator;
	FRotator PitchSourcePrevRot = FRotator::ZeroRotator;

	float YawSpeed = 0.f;
	float SmoothedYawSpeed = 0.f;
	float NormalizedYawSpeed = 0.f;
	float PitchSpeed = 0.f;
	float SmoothedPitchSpeed = 0.f;
	float NormalizedPitchSpeed = 0.f;

	/** 上次成功采样的世界时间；< 0 表示还没采过样 */
	float LastRotationSampleTime = -1.f;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float SpeedSmoothLerpSpeed = 1.f;
	UPROPERTY(EditAnywhere, Category = "Animation")
	float SpeedLerpToZeroSpeed = 30.f;

	/** 每秒角速度低于这个值就当作没动，直接把目标置 0（替代原来 == 0 的浮点精确比较） */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float RotationSpeedDeadZone = 0.5f;

	/**
	 * 两次采样的最大间隔（秒）。超过就当采样断了（URO、动画被隐藏/暂停、切关卡等），
	 * 这一帧直接置 0 并重新对齐，避免恢复瞬间算出一个天文数字。
	 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float MaxRotationSampleInterval = 0.25f;

	/** 打开后每帧打印角速度（只在本地控制端、非 Shipping 生效，用固定 key 替换而不是刷屏） */
	UPROPERTY(EditAnywhere, Category = "Animation|Debug")
	bool bDebugRotationSpeed = false;

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
	float GetFootIKOffset(const FName& InBoneName);
};
