// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeAnimInstanceBase.h"
#include "GeAnimInstanceLocoBase.generated.h"

class UCharacterMovementComponent;

/**
 * 所有「移动层」实例的公共基类。
 *
 * 它比 UGeAnimInstanceBase 多知道一件事：**当前是哪个移动模式**。
 * 模式和「快速飞行」都来自 UGeLocomotionComponent（GAS tag 推导出来的结果），
 * 主实例那份缓存原本在基类上，属于越界 —— 主 ABP 并不需要知道模式，
 * 真正按模式换姿态的是 layer ABP，所以这部分数据归这里。
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

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsGrounded() const { return LocomotionMode == EGeLocomotionMode::Ground; }

protected:
	float MaxSpeed = 0.f;
	FVector NormalizedVelocity = FVector::ZeroVector;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;

	/**********************************************************************/
	/*                                Fly                                 */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsFlying() const { return LocomotionMode == EGeLocomotionMode::Fly; }

	/** 归一化飞行速度。和 GetNormalizedVelocity() 完全等价，只是飞行层里读着顺手 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetFlySpeed() const { return NormalizedVelocity; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsFlyingFast() const { return bIsFlyingFast; }

protected:
	bool bIsFlyingFast = false;

	/**********************************************************************/
	/*                                Swim                                */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsSwimming() const { return LocomotionMode == EGeLocomotionMode::Swim; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsSwimMoving() const { return GetSpeed2D() != 0.f || (bIsDiving && IsMoving()); }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE FVector GetSwimSpeed() const { return NormalizedVelocity; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsDiving() const { return bIsDiving; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsSwimmingFast() const { return bIsSwimmingFast; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsSurfaceSwimming() const { return !bIsDiving; }

	/**
	 * 本地空间的前后速度（已按 MaxSpeed 归一化到 [-1,1]）。
	 * 2D 混合空间的 Y 轴（前后）接它。
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetSwimForwardSpeed() const { return NormalizedVelocity.X; }

	/**
	 * 本地空间的左右速度（已按 MaxSpeed 归一化到 [-1,1]）。
	 * 2D 混合空间的 X 轴（左右）接它。
	 *
	 * ⚠ 角色开着 bOrientRotationToMovement，会一直朝游动方向转身，
	 * 所以这个值在稳定游动时几乎恒为 0，Left/Right 样本吃不到权重。
	 * 想让「左看右看」明显，方向轴用下面的 GetSwimTurnAxis()。
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetSwimRightSpeed() const { return NormalizedVelocity.Y; }

	/**
	 * 本地空间的上下速度（已按 MaxSpeed 归一化到 [-1,1]）。
	 * 给「下潜 / 上浮」那条轴用（Underwater 混合空间的 Y 轴），不要和左右共用一根轴。
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetSwimVerticalSpeed() const { return NormalizedVelocity.Z; }

	/**
	 * 「方向」输入，范围 [-1,1]：0 = 正前，负 = 左，正 = 右。
	 * 由本地速度方向换算（方向角 / 90 再 clamp）。
	 *
	 * 同样受 bOrientRotationToMovement 影响（身体朝游动方向 → 长期≈0），
	 * 只在身体不朝移动方向时有意义（例如锁朝向的 strafe）。一般用 GetSwimTurnAxis() 更有效。
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	float GetSwimDirectionAxis() const;

	/**
	 * 「方向」输入，范围 [-1,1]：**由角色 Yaw 角速度换算**，左转 → 负，右转 → 正。
	 *
	 * 这是让 Swim 的 Left/Right 样本真正生效的推荐做法：
	 * 不看横向速度，所以 bOrientRotationToMovement 开着（身体朝游动方向）也照样有值。
	 * 直游 = 0（Forward），转得越快越靠向 Left/Right。
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	float GetSwimTurnAxis() const;

protected:
	bool bIsDiving = false;
	bool bIsSwimmingFast = false;

	/**********************************************************************/
	/*                                Climb                               */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsClimbing() const { return LocomotionMode == EGeLocomotionMode::Climb; }

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

	/** 调试：非 Shipping 下画出 LookAtLocation（黄球）和眼睛→目标连线，用来判断是「点没动」还是「节点没转」 */
	UPROPERTY(EditAnywhere, Category = "Animation|LookAt|Debug")
	bool bDebugLookAt = false;

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
