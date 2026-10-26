// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceLocoBase.h"

#include "Comps/Locomotions/GeLocomotionComponent.h"
#include "Data/GeLocomotionAnimSet.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGeAnimInstanceLocoBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn))
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}

	LocomotionComp = UGeLocomotionComponent::Find(OwnerPawn);
}

void UGeAnimInstanceLocoBase::CacheDerivedState()
{
	Super::CacheDerivedState();

	// 组件可能比 AnimInstance 晚就绪（换模型 / 重新 Possess），按需补查。
	if (!LocomotionComp)
	{
		LocomotionComp = UGeLocomotionComponent::Find(OwnerPawn);
	}

	// 动画资产也在这里（游戏线程）取一次，之后 BlendSpace 的取值函数只读这个缓存指针，
	// 动画工作线程不会去碰组件。
	LocomotionAnimSet = LocomotionComp ? LocomotionComp->GetAnimationSet() : nullptr;

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
}

UBlendSpace* UGeAnimInstanceLocoBase::GetFlyBlendSpace() const
{
	return LocomotionAnimSet ? LocomotionAnimSet->GetFlyBlendSpace() : nullptr;
}

UBlendSpace* UGeAnimInstanceLocoBase::GetSwimBlendSpace() const
{
	return LocomotionAnimSet ? LocomotionAnimSet->GetSwimBlendSpace() : nullptr;
}

UBlendSpace* UGeAnimInstanceLocoBase::GetClimbBlendSpace() const
{
	return LocomotionAnimSet ? LocomotionAnimSet->GetClimbBlendSpace() : nullptr;
}
