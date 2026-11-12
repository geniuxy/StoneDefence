// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceLocoBase.h"

#include "Comps/Locomotions/GeLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

void UGeAnimInstanceLocoBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn))
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}
}

void UGeAnimInstanceLocoBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// LookAt / Lean 只依赖 Pawn 和基类缓存的 LocalVelocity，不依赖 MovementComp，
	// 所以放在最前面算：没有 CharacterMovement（预览、非 Character 的 Pawn）时也有值。
	UpdateLookAtLocation();
	UpdateLean(DeltaSeconds);

	// 移动模式 + 是否快速飞行：这两份缓存从 UGeAnimInstanceBase 搬到了本类 ——
	// layer 实例才知道「现在该播哪一套姿态」，主实例不需要。
	// 数据源仍然是 UGeLocomotionComponent（GAS tag 推导结果），这里只是每帧同步一次。
	if (const UGeLocomotionComponent* LocomotionComp = UGeLocomotionComponent::Find(OwnerPawn))
	{
		bIsFlyingFast = LocomotionComp->IsFlyingFast();
		bIsSwimmingFast = LocomotionComp->IsSwimmingFast();
		bIsDiving = LocomotionComp->IsUnderwater();
	}
	else
	{
		bIsFlyingFast = false;
		bIsSwimmingFast = false;
		bIsDiving = false;
	}

	if (!OwnerMovementComp)
	{
		if (const ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn))
		{
			OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
		}
	}

	if (!OwnerMovementComp) return;

	// GetMaxSpeed() 会按当前 MovementMode 返回 MaxWalkSpeed / MaxFlySpeed / MaxSwimSpeed，
	// 所以不用自己 switch。
	MaxSpeed = OwnerMovementComp->GetMaxSpeed();

	// LocalVelocity 已经在基类转成本地空间了，这里只做归一化。
	// 等价于逐轴 FMath::GetMappedRangeValueClamped(-Max, Max, V)。
	if (MaxSpeed > KINDA_SMALL_NUMBER)
	{
		NormalizedVelocity = LocalVelocity / MaxSpeed;
		NormalizedVelocity.X = FMath::Clamp(NormalizedVelocity.X, -1.f, 1.f);
		NormalizedVelocity.Y = FMath::Clamp(NormalizedVelocity.Y, -1.f, 1.f);
		NormalizedVelocity.Z = FMath::Clamp(NormalizedVelocity.Z, -1.f, 1.f);
	}
	else
	{
		NormalizedVelocity = FVector::ZeroVector;
	}

	// ── 各模式的开关 ────────────────────────────────────────────────

	// 攀爬：向上 / 向下爬
	constexpr float ClimbVerticalDeadZone = 0.15f;
	bIsClimbUp = LocalVelocity.Z > ClimbVerticalDeadZone;
	bIsClimbDown = LocalVelocity.Z < -ClimbVerticalDeadZone;
}

void UGeAnimInstanceLocoBase::UpdateLookAtLocation()
{
	// Look At 节点的 Look At Location 要的是世界空间的「点」，不是方向，所以这里把
	// 瞄准方向折算成「眼睛前方 LookAtDistance 处的一个点」。头骨朝这个点看，
	// 等价于头部跟随准心：水平跟 Yaw，低头/抬头跟 Pitch。
	FVector AimOrigin = FVector::ZeroVector;
	FRotator AimRotation = FRotator::ZeroRotator;

	if (OwnerPawn)
	{
		// 眼睛位置：ACharacter::GetPawnViewLocation 已经带上 BaseEyeHeight，和头骨基本重合。
		// 瞄准方向：和 GetPitchSourceRotation() 用同一个源（本地控制端 = ControlRotation，
		// 模拟端 = 复制过来的 RemoteViewPitch），所以抬头/低头能如实传到头上。
		AimOrigin = OwnerPawn->GetPawnViewLocation();
		AimRotation = OwnerPawn->GetBaseAimRotation();
	}
	else if (OwnerSkeletalMeshComp)
	{
		// 没有 Pawn（Persona 预览等）：用 Mesh 自己的朝向兜底。
		// 不兜底的话这个值会一直是 (0,0,0)，Look At 节点会把头扭向世界原点。
		AimOrigin = OwnerSkeletalMeshComp->GetComponentLocation();
		AimRotation = OwnerSkeletalMeshComp->GetComponentRotation();
	}
	else
	{
		LookAtLocation = FVector::ZeroVector;
		return;
	}

	// 距离取远一点（默认 100m）：眼睛和头骨之间的位置差换算成角度会变成几度的
	// 抬/低头误差，距离越远这个误差越小（方向趋近于纯瞄准方向）。
	LookAtLocation = AimOrigin + AimRotation.Vector() * LookAtDistance;

#if !UE_BUILD_SHIPPING
	if (bDebugLookAt)
	{
		if (const UWorld* World = GetWorld())
		{
			// 黄球 = 目标点，白线 = 眼睛到目标；Debug Thickness 在 Game Viewport 里看得见
			DrawDebugSphere(World, LookAtLocation, 12.f, 12, FColor::Yellow, false, 0.f, 0, 1.f);
			DrawDebugLine(World, AimOrigin, LookAtLocation, FColor::White, false, 0.f, 0, 0.5f);
			DrawDebugPoint(World, AimOrigin, 8.f, FColor::Cyan, false, 0.f);
		}
	}
#endif
}

void UGeAnimInstanceLocoBase::UpdateLean(float DeltaSeconds)
{
	// 参考 ABP（SuperheroFlight 的 GetLeanParameterFunc）的算法结构：
	//   取速度 -> 转本地空间 -> 按速度大小映射到 0..1 -> FInterpTo 平滑。
	// LocalVelocity 是基类已经算好的本地空间速度：X 前后 / Y 左右 / Z 上下，
	// 所以「左右倾斜」直接取 Y、「上下倾斜」取 Z，和参考 ABP 里
	// LessLess_VectorRotator + MapRangeClamped + FInterpTo 那串节点等价。

	float TargetX = 0.f;
	float TargetY = 0.f;

	if (LocalVelocity.Size() > KINDA_SMALL_NUMBER)
	{
		// 用单位方向而不是原始速度：飞得再快倾斜也不会超过 1。
		const FVector Direction = LocalVelocity / LocalVelocity.Size();

		// 低速衰减，等价于 MapRangeClamped(Speed, 0, LeanFullSpeed, 0, 1)：
		// 悬停时倾斜为 0，到 LeanFullSpeed 时满幅度。
		const float SpeedAlpha = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, LeanFullSpeed), FVector2D(0.f, 1.f), LocalVelocity.Size());

		TargetX = Direction.Y * SpeedAlpha;
		TargetY = Direction.Z * SpeedAlpha;
	}

	// 死区：匀速直线飞行时速度方向的细微抖动不该带出倾斜。
	if (FMath::Abs(TargetX) < LeanDeadZone) TargetX = 0.f;
	if (FMath::Abs(TargetY) < LeanDeadZone) TargetY = 0.f;

	LeanX = FMath::Clamp(FMath::FInterpTo(LeanX, TargetX, DeltaSeconds, LeanInterpSpeed), -1.f, 1.f);
	LeanY = FMath::Clamp(FMath::FInterpTo(LeanY, TargetY, DeltaSeconds, LeanInterpSpeed), -1.f, 1.f);
}
