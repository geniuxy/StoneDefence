// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceBase.h"

#include "GeniuxyDebugHelper.h"
#include "Comps/Locomotions/GeLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "SimpleAdvancedAnimationBPLibrary.h"
#include "Engine/World.h"

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

	// 关键：把「上一帧采样」对齐到当前朝向。
	// 不做这一步，BodyPrevRot 会是 (0,0,0)，首帧 BodyRotDelta 就是角色当前整个朝向，
	// YawSpeed 会算出几千度/秒的尖峰，然后被平滑慢慢拖回来。
	ResetRotationSpeedTracking();
}

void UGeAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

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
		bIsFlyingFast = Comp->IsFlyingFast();
	}

	/**********************************************************************/
	/*                        Yaw / Pitch 角速度                          */
	/**********************************************************************/
	// Yaw 用 Actor 朝向：地面移动时 Actor 只有 Yaw 在变（bOrientRotationToMovement）。
	// Pitch 用 GetPitchSourceRotation()：抬头/低头只在 Controller 的 ControlRotation 上，
	// Actor 的 Pitch 被 CMC / FaceRotation 钉死为 0，用它会永远算出 0。
	const UWorld* World = GetWorld();
	const float Now = World ? World->GetTimeSeconds() : 0.f;

	// DeltaSeconds 可能为 0（暂停/同一帧内多次更新），直接除会出 inf/NaN。
	// 采样间隔过大说明动画中间停过（URO、隐藏、切关卡），这一帧的差值不可信。
	const bool bSampleValid =
		LastRotationSampleTime >= 0.f &&
		DeltaSeconds > KINDA_SMALL_NUMBER &&
		(Now - LastRotationSampleTime) <= MaxRotationSampleInterval;

	const FRotator BodyRot = OwnerPawn->GetActorRotation();
	const FRotator PitchRot = GetPitchSourceRotation();

	if (bSampleValid)
	{
		// FindDeltaAngleDegrees = 环绕安全的两角之差（等价于 NormalizedDeltaRotator 的逐轴版本）
		YawSpeed = FMath::FindDeltaAngleDegrees(BodyPrevRot.Yaw, BodyRot.Yaw) / DeltaSeconds;
		PitchSpeed = FMath::FindDeltaAngleDegrees(PitchSourcePrevRot.Pitch, PitchRot.Pitch) / DeltaSeconds;
	}
	else
	{
		// 采样断了就重新起步，不要从旧值滑过去
		YawSpeed = 0.f;
		PitchSpeed = 0.f;
		SmoothedYawSpeed = 0.f;
		SmoothedPitchSpeed = 0.f;
	}

	BodyPrevRot = BodyRot;
	PitchSourcePrevRot = PitchRot;
	LastRotationSampleTime = Now;

	// 先按死区判定目标值，再决定用哪个插值速度。
	const float YawTarget = FMath::Abs(YawSpeed) < RotationSpeedDeadZone ? 0.f : YawSpeed;
	const float PitchTarget = FMath::Abs(PitchSpeed) < RotationSpeedDeadZone ? 0.f : PitchSpeed;

	SmoothedYawSpeed = FMath::FInterpTo(
		SmoothedYawSpeed, YawTarget, DeltaSeconds,
		YawTarget == 0.f ? SpeedLerpToZeroSpeed : SpeedSmoothLerpSpeed);

	SmoothedPitchSpeed = FMath::FInterpTo(
		SmoothedPitchSpeed, PitchTarget, DeltaSeconds, SpeedSmoothLerpSpeed);

#if !UE_BUILD_SHIPPING
	if (bDebugRotationSpeed && OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		Debug::Print(
			FString::Printf(
				TEXT("%s  Yaw %.1f/%.1f  Pitch %.1f/%.1f"),
				*GetClass()->GetName(), YawSpeed, SmoothedYawSpeed, PitchSpeed, SmoothedPitchSpeed),
			9001, 0.f, FColor::Yellow);
	}
#endif

	// 脚部 IK：角色级开关（UGeAnimationSetup -> UGeLocomotionComponent）优先；
	// 拿不到组件时（例如预览 Actor）才回退到本类 Class Defaults 的 bCanFootIK。
	// 腾空时一律关掉。
	const bool bFootIKAllowed = Comp ? Comp->IsFootIKEnabled() : bCanFootIK;
	bFootIK = bFootIKAllowed && !bIsInAir && LocomotionMode != EGeLocomotionMode::Fly;
}

void UGeAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// 跑在动画工作线程：只能读上面缓存好的成员，绝对不要在这里读 Actor / Component。
	// CalcFootIKOffset();
}

FRotator UGeAnimInstanceBase::GetPitchSourceRotation() const
{
	if (!OwnerPawn) return FRotator::ZeroRotator;

	// 说明见头文件里的注释：Actor 的 Pitch 在本项目恒为 0，抬头/低头只在 Controller 上。
	return OwnerPawn->GetBaseAimRotation();
}

void UGeAnimInstanceBase::ResetRotationSpeedTracking()
{
	YawSpeed = 0.f;
	PitchSpeed = 0.f;
	SmoothedYawSpeed = 0.f;
	SmoothedPitchSpeed = 0.f;

	BodyPrevRot = OwnerPawn ? OwnerPawn->GetActorRotation() : FRotator::ZeroRotator;
	PitchSourcePrevRot = GetPitchSourceRotation();

	const UWorld* World = GetWorld();
	LastRotationSampleTime = World ? World->GetTimeSeconds() : -1.f;
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
