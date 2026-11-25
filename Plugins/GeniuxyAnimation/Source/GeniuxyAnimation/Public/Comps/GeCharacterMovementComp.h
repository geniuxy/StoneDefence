// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GeCharacterMovementComp.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GENIUXYANIMATION_API UGeCharacterMovementComp : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UGeCharacterMovementComp();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
	) override;

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	/** 保留的原本攀爬前的OrientRotationToMovement状态 */
	bool bSavedOrientRotationToMovement = true;

	/** 攀爬前记下的胶囊半高（BeginPlay 里初始化；为 0 表示还没记过，退出攀爬时不要拿它恢复） */
	float CachedCapsuleHalfHeight = 0.f;

	/**********************************************************************/
	/*                          Speed / Accel                             */
	/**********************************************************************/
public:
	/**
	 * 让 MaxClimbSpeed / MaxClimbAcceleration / MaxBreakClimbDeceleration 真正生效。
	 *
	 * 为什么必须覆盖：PhysClimb 里那两句引擎调用读的是虚函数，不是这几个 UPROPERTY ——
	 *   - CalcVelocity()        → GetMaxSpeed()（速度上限）、GetMaxAcceleration()（加速上限）
	 *   - ScaleInputAcceleration → GetMaxAcceleration()（输入 → Acceleration 的放大倍数）
	 * 不覆盖的话，攀爬用的还是 MaxWalkSpeed / MaxAcceleration（走路那套）。
	 */
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;

protected:
	/** 当前是不是在攀爬自定义模式（MOVE_Custom + MOVE_Climb） */
	bool IsClimbing() const;

	/**********************************************************************/
	/*                               Climb                                */
	/**********************************************************************/
public:
	bool TraceClimbableSurfaces();

	FHitResult TraceFromEyeHeight(
		float TraceDistance,
		float TraceStartOffset = 0.f,
		bool bShowDebugShape = false,
		bool bDrawPersistantShapes = false
	);

protected:
	virtual void PhysClimb(float DeltaTime, int32 Iterations);

	void ProcessClimbableSurfaceInfo();

	bool CheckShouldStopClimbing();

	bool CheckHasReachedFloor();

	void StopClimbing();

	FQuat GetClimbRotation(float DeltaTime);

	FVector GetUnRotatedClimbVelocity() const;

	void SnapMovementToClimbableSurfaces(float DeltaTime);

	TArray<FHitResult> DoCapsuleTraceMultiByObject(
		const FVector& Start,
		const FVector& End,
		bool bShowDebugShape = false,
		bool bDrawPersistantShapes = false
	);

	FHitResult DoLineTraceSingleByObject(
		const FVector& Start,
		const FVector& End,
		bool bShowDebugShape = false,
		bool bDrawPersistantShapes = false
	);

protected:
	TArray<FHitResult> ClimbableSurfacesTracedResults;

	FVector CurrentClimbableSurfaceLocation;
	FVector CurrentClimbableSurfaceNormal;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	TArray<TEnumAsByte<EObjectTypeQuery>> ClimbableSurfaceTraceTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbCapsuleTraceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbCapsuleTraceHalfHeight = 72.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float MaxBreakClimbDeceleration = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float MaxClimbSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float MaxClimbAcceleration = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	UAnimMontage* IdleToClimbMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	UAnimMontage* ClimbToTopMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbDownWalkableSurfaceTraceOffset = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbDownLedgeTraceOffset = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	UAnimMontage* ClimbDownLedgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	UAnimMontage* VaultMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	UAnimMontage* HopUpMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	UAnimMontage* HopDownMontage;

public:
	/** 当前攀爬墙面的法线（朝角色这一侧）。只在攀爬模式期间有效，其它时候是零向量 */
	FORCEINLINE FVector GetClimbableSurfaceNormal() const { return CurrentClimbableSurfaceNormal; }
};
