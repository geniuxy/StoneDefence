// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceLocoBase.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGeAnimInstanceLocoBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn))
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}
}

void UGeAnimInstanceLocoBase::CacheDerivedState()
{
	Super::CacheDerivedState();

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
	// 全部基于已经缓存好的 LocalVelocity，不读 Actor，工作线程安全。
	// 三个阈值只是「判定死区」，避免贴地/贴墙时因微小速度抖动来回切换。

	// 飞行：明显在上升 / 下降
	constexpr float FlyVerticalDeadZone = 0.15f;
	bIsAscending = LocalVelocity.Z > FlyVerticalDeadZone;
	bIsDescending = LocalVelocity.Z < -FlyVerticalDeadZone;

	// 游泳：向下游算潜水
	constexpr float DiveDeadZone = 0.2f;
	bIsDiving = LocalVelocity.Z < -DiveDeadZone;

	// 攀爬：向上 / 向下爬
	constexpr float ClimbVerticalDeadZone = 0.15f;
	bIsClimbUp = LocalVelocity.Z > ClimbVerticalDeadZone;
	bIsClimbDown = LocalVelocity.Z < -ClimbVerticalDeadZone;
}
