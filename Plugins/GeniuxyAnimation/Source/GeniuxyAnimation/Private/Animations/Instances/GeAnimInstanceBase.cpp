// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceBase.h"

#include "Comps/Locomotions/GeLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "SimpleAdvancedAnimationBPLibrary.h"

void UGeAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerSkeletalMeshComp = GetSkelMeshComponent();
	OwnerPawn = TryGetPawnOwner();

	// 注意：这里不再注册 GAS tag 事件。
	// tag -> 模式 的转换统一由 UGeLocomotionComponent 负责，AnimInstance 只读结果，
	// 避免同一个 tag 被注册两次、以及实例销毁时忘记反注册造成的悬空委托。

	bFootIK = bCanFootIK;
}

void UGeAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	CacheOwnerState();
	CacheDerivedState();
}

void UGeAnimInstanceBase::CacheOwnerState()
{
	// 全部在游戏线程完成，写进成员变量，工作线程只读。
	if (!OwnerPawn) return;

	WorldVelocity = OwnerPawn->GetVelocity();
	Speed = WorldVelocity.Size();
	Speed2D = WorldVelocity.Size2D();

	// 转成角色本地空间：混合空间接 X/Y 时不用再自己 UnrotateVector
	const FRotator OwnerRotation = OwnerPawn->GetActorRotation();
	LocalVelocity = OwnerRotation.UnrotateVector(WorldVelocity);

	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn))
	{
		const UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement();
		bIsInAir = MovementComp ? MovementComp->IsFalling() : false;
	}

	// 移动模式 / 战斗状态：本类不持有组件指针（那两个成员在 UGeAnimInstanceLocoBase 上），每帧局部取一次即可。
	if (const UGeLocomotionComponent* Comp = UGeLocomotionComponent::Find(OwnerPawn))
	{
		LocomotionMode = Comp->GetMode();
		bInCombat = Comp->IsInCombat();
	}

	// 足部 IK：腾空时关掉
	bFootIK = bCanFootIK && !bIsInAir;
}

void UGeAnimInstanceBase::CacheDerivedState()
{
	// 派生类在这里追加自己的缓存逻辑
}

void UGeAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// 跑在动画工作线程：只能读上面缓存好的成员，绝对不要在这里读 Actor / Component。
	// CalcFootIKOffset();
}

void UGeAnimInstanceBase::InitFootIKId(ACharacter* OwnerCharacter)
{
	BoneNames.Empty();
	if (GetIsFootIK())
	{
		BoneNames.Add(LeftBoneName);
		BoneNames.Add(RightBoneName);

		FootIKId = USimpleAdvancedAnimationBPLibrary::CreateFootIK(OwnerCharacter, BoneNames);
	}
}

void UGeAnimInstanceBase::CalcFootIKOffset()
{
	if (GetIsFootIK() && FootIKId != INDEX_NONE)
	{
		TArray<float> OffsetArray;

		float LOffset = GetFootIKOffset(LeftBoneName);
		float ROffset = GetFootIKOffset(RightBoneName);

		OffsetArray.Add(LOffset);
		OffsetArray.Add(ROffset);

		ButtZOffset = USimpleAdvancedAnimationBPLibrary::GetButtZOffset(OffsetArray);

		LeftOffset = -(ButtZOffset - LOffset);
		RightOffset = ButtZOffset - ROffset;
	}
}

float UGeAnimInstanceBase::GetFootIKOffset(const FName& InBoneName)
{
	if (FootIKId != INDEX_NONE)
	{
		return USimpleAdvancedAnimationBPLibrary::FindOffset(FootIKId,InBoneName);
	}

	return 0.0f;
}
