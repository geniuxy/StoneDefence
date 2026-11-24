// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/GeCharacterMovementComp.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GeniuxyAnimationType.h"
#include "Components/CapsuleComponent.h"
#include "Comps/Locomotions/GeLocomotionComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Tags/StatsTags.h"

UGeCharacterMovementComp::UGeCharacterMovementComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGeCharacterMovementComp::BeginPlay()
{
	Super::BeginPlay();
}


void UGeCharacterMovementComp::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGeCharacterMovementComp::PhysCustom(float DeltaTime, int32 Iterations)
{
	if (CustomMovementMode == ECustomMovementMode::MOVE_Climb)
	{
		PhysClimb(DeltaTime, Iterations);
		return;
	}

	Super::PhysCustom(DeltaTime, Iterations);
}

void UGeCharacterMovementComp::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	const bool bWasClimb = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb;
	const bool bIsClimb  = IsClimbing();

	if (bIsClimb && !bWasClimb)
	{
		bSavedOrientRotationToMovement = bOrientRotationToMovement;   // 记住原值，别假设是 true
		bOrientRotationToMovement = false;                          // 交给 PhysClimb 锁 yaw

		CachedCapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(50.f);
	}
	else if (bWasClimb && !bIsClimb)
	{
		bOrientRotationToMovement = bSavedOrientRotationToMovement;  // 恢复

		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(CachedCapsuleHalfHeight);
	}
}

/**********************************************************************/
/*                          Speed / Accel                             */
/**********************************************************************/
float UGeCharacterMovementComp::GetMaxSpeed() const
{
	// CalcVelocity 用它做速度上限；IsExceedingMaxSpeed 也用它
	return IsClimbing() ? MaxClimbSpeed : Super::GetMaxSpeed();
}

float UGeCharacterMovementComp::GetMaxAcceleration() const
{
	// ScaleInputAcceleration（输入 → Acceleration 的放大倍数）和 CalcVelocity 的 MaxAccel 都用它
	return IsClimbing() ? MaxClimbAcceleration : Super::GetMaxAcceleration();
}

float UGeCharacterMovementComp::GetMaxBrakingDeceleration() const
{
	return IsClimbing() ? MaxBreakClimbDeceleration : Super::GetMaxBrakingDeceleration();
}

bool UGeCharacterMovementComp::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;
}

/**********************************************************************/
/*                               Climb                                */
/**********************************************************************/
void UGeCharacterMovementComp::PhysClimb(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME || !CharacterOwner) return;

	/*Process all the climbable surfaces info*/
	TraceClimbableSurfaces();
	ProcessClimbableSurfaceInfo();

	/*Check if we should stop climbing*/
	if (CheckShouldStopClimbing() || CheckHasReachedFloor())
	{
		StopClimbing();
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		//Define the max climb speed and acceleration
		CalcVelocity(DeltaTime, 0.f, true, MaxBreakClimbDeceleration);
	}

	ApplyRootMotionToVelocity(DeltaTime);

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * DeltaTime;
	FHitResult Hit(1.f);

	//Handle climb rotation
	SafeMoveUpdatedComponent(Adjusted, GetClimbRotation(DeltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
		HandleImpact(Hit, DeltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
	}

	/* Snap(吸附) movement to climbable surfaces */
	SnapMovementToClimbableSurfaces(DeltaTime);
	//
	// if (CheckHasReachedLedge())
	// {
	// 	PlayClimbMontage(ClimbToTopMontage);
	// }
}

bool UGeCharacterMovementComp::TraceClimbableSurfaces()
{
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	ClimbableSurfacesTracedResults = DoCapsuleTraceMultiByObject(Start, End);

	return !ClimbableSurfacesTracedResults.IsEmpty();
}

void UGeCharacterMovementComp::ProcessClimbableSurfaceInfo()
{
	CurrentClimbableSurfaceLocation = FVector::ZeroVector;
	CurrentClimbableSurfaceNormal = FVector::ZeroVector;

	if (ClimbableSurfacesTracedResults.IsEmpty()) return;

	for (const FHitResult& TracedHitResult : ClimbableSurfacesTracedResults)
	{
		CurrentClimbableSurfaceLocation += TracedHitResult.ImpactPoint;
		CurrentClimbableSurfaceNormal += TracedHitResult.ImpactNormal;
	}

	CurrentClimbableSurfaceLocation /= ClimbableSurfacesTracedResults.Num();
	CurrentClimbableSurfaceNormal = CurrentClimbableSurfaceNormal.GetSafeNormal();
}

bool UGeCharacterMovementComp::CheckShouldStopClimbing()
{
	if (ClimbableSurfacesTracedResults.IsEmpty()) return true;

	const float DotResult = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::UpVector);
	const float DegreeDiff = FMath::RadiansToDegrees(FMath::Acos(DotResult));

	if (DegreeDiff <= 60.f)
	{
		return true;
	}

	return false;
}

bool UGeCharacterMovementComp::CheckHasReachedFloor()
{
	const FVector DownVector = -UpdatedComponent->GetUpVector();
	const FVector StartOffset = DownVector * 50.f;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + DownVector;

	TArray<FHitResult> PossibleFloorHits = DoCapsuleTraceMultiByObject(Start, End);

	if (PossibleFloorHits.IsEmpty()) return false;

	for (const FHitResult& PossibleFloorHit : PossibleFloorHits)
	{
		const bool bFloorReached =
			FVector::Parallel(-PossibleFloorHit.ImpactNormal, FVector::UpVector) &&
			GetUnRotatedClimbVelocity().Z < -10.f;

		if (bFloorReached)
		{
			return true;
		}
	}

	return false;
}

void UGeCharacterMovementComp::StopClimbing()
{
	if (UGeLocomotionComponent* Loco = UGeLocomotionComponent::Find(CharacterOwner))
	{
		Loco->PopMode();          // 或 SetMode(EGeLocomotionMode::Ground)
	}

	// 同时把攀爬 tag 清掉（它是 montage 里的 AN 加的 GE）
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CharacterOwner))
	{
		ASC->RemoveActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(
			FGameplayTagContainer(StatsTags::Ge_Stats_Climbing))
		);
	}
}

FQuat UGeCharacterMovementComp::GetClimbRotation(float DeltaTime)
{
	// 四元数（Quaternion）是一种用于表示三维空间中旋转的数学工具
	// 不作处理的话，AActor的RootComponent就是Component的UpdatedComponent
	const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();
	// 如果当前物体有动画根运动或根运动覆盖，则保持当前旋转不变。
	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return CurrentQuat;
	}

	// 如果没有上述特殊情况，则计算一个目标旋转，使物体的 X 轴指向攀爬表面法线的反方向。
	const FQuat TargetQuat = FRotationMatrix::MakeFromX(-CurrentClimbableSurfaceNormal).ToQuat();

	return FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, 5.f);
}

FVector UGeCharacterMovementComp::GetUnRotatedClimbVelocity() const
{
	// 假设一个角色在攀爬一个倾斜的墙壁，其速度向量在局部坐标系中是垂直向上的（相对于角色的视角）
	// 通过这个函数，可以将这个局部速度向量转换到世界坐标系中
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}

void UGeCharacterMovementComp::SnapMovementToClimbableSurfaces(float DeltaTime)
{
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

	// 计算角色当前位置到攀爬表面的投影向量
	const FVector ProjectedCharacterToSurface =
		(CurrentClimbableSurfaceLocation - ComponentLocation).ProjectOnTo(ComponentForward);

	// 根据投影向量计算一个“吸附”向量，方向为攀爬表面法线的反方向，长度为投影向量的长度
	const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedCharacterToSurface.Length();

	// 将角色沿着这个“吸附”向量移动，使其“吸附”到攀爬表面上
	UpdatedComponent->MoveComponent(
		SnapVector * DeltaTime * MaxClimbSpeed,
		UpdatedComponent->GetComponentQuat(),
		true
	);
}

TArray<FHitResult> UGeCharacterMovementComp::DoCapsuleTraceMultiByObject(
	const FVector& Start, const FVector& End, bool bShowDebugShape, bool bDrawPersistantShapes)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if (bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if (bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		ClimbCapsuleTraceRadius,
		ClimbCapsuleTraceHalfHeight,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutCapsuleTraceHitResults,
		false
	);

	return OutCapsuleTraceHitResults;
}

FHitResult UGeCharacterMovementComp::DoLineTraceSingleByObject(
	const FVector& Start, const FVector& End, bool bShowDebugShape, bool bDrawPersistantShapes)
{
	FHitResult OutHit;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if (bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if (bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutHit,
		false
	);

	return OutHit;
}
