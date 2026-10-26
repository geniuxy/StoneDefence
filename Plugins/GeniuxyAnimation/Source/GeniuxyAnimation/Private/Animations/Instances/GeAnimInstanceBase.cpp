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

	// 按骨架自动判定有没有 IK 脚骨骼（Tubaki 有 ik_foot_*，Cleric 没有）。
	// 这样主 ABP 的 FootIK 分支就不依赖「按武将手填类默认值」——那在架构上本来就做不到。
	DetectIKBoneSupport();

	bFootIK = bCanFootIK;
}

void UGeAnimInstanceBase::DetectIKBoneSupport()
{
	if (!bAutoDetectIKBone) return;

	if (!OwnerSkeletalMeshComp)
	{
		OwnerSkeletalMeshComp = GetSkelMeshComponent();
	}
	if (!OwnerSkeletalMeshComp) return;

	// USkinnedMeshComponent::GetBoneIndex 内部就是
	// GetSkinnedAsset()->GetRefSkeleton().FindBoneIndex(BoneName)，
	// 不依赖骨骼映射是否已经初始化，初始化阶段调用是安全的。
	//
	// 两具骨架都有 foot_l / foot_r，区别只在带 IK 的骨架额外有 ik_foot_l / ik_foot_r，
	// 所以用 IK 骨骼名判断，而不是 foot_l / foot_r。
	const bool bHasLeftIK = OwnerSkeletalMeshComp->GetBoneIndex(IKFootLeftBoneName) != INDEX_NONE;
	const bool bHasRightIK = OwnerSkeletalMeshComp->GetBoneIndex(IKFootRightBoneName) != INDEX_NONE;

	bHasIKBone = bHasLeftIK && bHasRightIK;
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

	// 移动模式 / 战斗状态 / 脚部 IK 开关：本类不持有组件指针
	// （LocomotionComp 成员在 UGeAnimInstanceLocoBase 上），每帧局部取一次即可。
	const UGeLocomotionComponent* Comp = UGeLocomotionComponent::Find(OwnerPawn);
	if (Comp)
	{
		LocomotionMode = Comp->GetMode();
		bInCombat = Comp->IsInCombat();
	}

	// 脚部 IK：角色级开关（UGeAnimationSetup -> UGeLocomotionComponent）优先；
	// 拿不到组件时（例如预览 Actor）才回退到本类 Class Defaults 的 bCanFootIK。
	// 腾空时一律关掉。
	const bool bFootIKAllowed = Comp ? Comp->IsFootIKEnabled() : bCanFootIK;
	bFootIK = bFootIKAllowed && !bIsInAir;
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
