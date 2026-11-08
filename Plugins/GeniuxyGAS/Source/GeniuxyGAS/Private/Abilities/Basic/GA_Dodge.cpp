// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Dodge.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GeniuxyDebugHelper.h"
#include "Tags/StatsTags.h"

namespace
{
	const TCHAR* Key_PreLoad = TEXT("Dodge.PreLoad");
	const TCHAR* Key_Dash = TEXT("Dodge.Dash");
	const TCHAR* Key_MontageCancelled = TEXT("Dodge.MontageCancelled");
	const TCHAR* Key_Blocked = TEXT("Dodge.Blocked");

	FString DodgeDirectionToString(EDodgeDirection InDirection)
	{
		switch (InDirection)
		{
		case EDodgeDirection::Forward:
			return TEXT("Forward");
		case EDodgeDirection::Backward:
			return TEXT("Backward");
		case EDodgeDirection::Left:
			return TEXT("Left");
		case EDodgeDirection::Right:
			return TEXT("Right");
		default:
			break;
		}
		return TEXT("Unknown");
	}
}

UGA_Dodge::UGA_Dodge()
{
	// 闪避要在本地立刻响应：蒙太奇和位移都是预测的
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Dodging);
	ActivationBlockedTags.AddTag(StatsTags::Ge_Stats_Dodging);

	// 死了不能闪避
	ActivationBlockedTags.AddTag(StatsTags::Ge_Stats_Dead);

	// 「什么时候能闪」是条件式的（地面随时 / 飞行中只有加速飞行），表达不成静态 Tag，
	// 放在下面的 CanActivateAbility 里判断。

	// 默认蒙太奇 Tag
	GroundDodgeMontageTags.Add(EDodgeDirection::Forward, MontageTags::Ge_Montage_Dodge_Forward);
	GroundDodgeMontageTags.Add(EDodgeDirection::Backward, MontageTags::Ge_Montage_Dodge_Backward);
	GroundDodgeMontageTags.Add(EDodgeDirection::Left, MontageTags::Ge_Montage_Dodge_Left);
	GroundDodgeMontageTags.Add(EDodgeDirection::Right, MontageTags::Ge_Montage_Dodge_Right);

	FlyDodgeMontageTags.Add(EDodgeDirection::Forward, MontageTags::Ge_Montage_Dodge_Fly_Up);
	FlyDodgeMontageTags.Add(EDodgeDirection::Backward, MontageTags::Ge_Montage_Dodge_Fly_Down);
	FlyDodgeMontageTags.Add(EDodgeDirection::Left, MontageTags::Ge_Montage_Dodge_Fly_Left);
	FlyDodgeMontageTags.Add(EDodgeDirection::Right, MontageTags::Ge_Montage_Dodge_Fly_Right);
}

void UGA_Dodge::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 方向先算，基类的 ActivateAbility 里会调 PreLoadMontage()，而蒙太奇是按方向取的，所以顺序不能反。
	if (!BuildDodgeContext())
	{
		K2_EndAbility();
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 基类在 PreLoadMontage() 失败时会 K2_EndAbility()，但它不会 return，
	// 派生类必须自己判活，否则会在一个已经结束的能力上继续建任务。
	if (!IsActive())
	{
		return;
	}

	if (!K2_CommitAbility())
	{
		// 冷却 / 消耗没通过（闪避一般会在蓝图里配一个 Cooldown GE）
		K2_EndAbility();
		return;
	}

	// 朝向：默认不动角色（方向相对角色自身算，闪避时 CMC 会自然把他转向位移方向）。
	// 只有走「相机相对」方案时，才需要把角色朝到相机方向并锁住朝向。
	if (bFaceViewDirectionOnDodge)
	{
		FaceViewDirection();
	}
	if (bLockRotationDuringDodge)
	{
		SetRotationLocked(true);
	}

	bMontageFinished = true;
	bDashFinished = true;

	if (DodgeMontage)
	{
		PlayDodgeMontage();
	}

	const bool bMontageDrivesMovement = bUseMontageRootMotion && DodgeMontage && DodgeMontage->HasRootMotion();
	if (!bMontageDrivesMovement)
	{
		// 没蒙太奇也照样位移：美术资源还没配的时候，四向逻辑也能先验证
		StartFallbackDash();
	}

	TryFinishAbility();
}

void UGA_Dodge::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	SetRotationLocked(false);

	DodgeMontage = nullptr;
	bMontageFinished = true;
	bDashFinished = true;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_Dodge::PreLoadMontage()
{
	DodgeMontage = nullptr;

	const FGameplayTag MontageTag = GetDodgeMontageTag();

	// 缺蒙太奇时只告警、继续用兜底位移，方便先联调方向和手感。
	if (!MontageTag.IsValid())
	{
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Dodge] %s 没有配置 %s 方向的蒙太奇 Tag，本次闪避只有位移"),
				*GetName(), *DodgeDirectionToString(DodgeDirection)),
			Key_PreLoad, 8.f, FColor::Yellow);
		return true;
	}

	const TSoftObjectPtr<UAnimMontage> SoftMontage = GetMontageByTag(MontageTag);
	if (SoftMontage.IsNull())
	{
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Dodge] %s 的 AnimationSet 里没有 Tag %s 对应的蒙太奇，本次闪避只有位移"),
				*GetName(), *MontageTag.ToString()),
			Key_PreLoad, 8.f, FColor::Yellow);
		return true;
	}

	// 注意：不能只凭 ASC 的 AreMontagesLoaded() 判断「已加载」，
	// 这里的 LoadMontageByTag 内部是 Get() -> 拿不到再 LoadSynchronous()。
	DodgeMontage = LoadMontageByTag(MontageTag);
	if (!DodgeMontage)
	{
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Dodge] %s 的蒙太奇 %s（%s）加载失败，本次闪避只有位移"),
				*GetName(), *MontageTag.ToString(), *SoftMontage.ToSoftObjectPath().ToString()),
			Key_PreLoad, 8.f, FColor::Yellow);
		return true;
	}

	return true;
}

bool UGA_Dodge::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	// 死亡 / 冷却 / 闪避中 这些交给 GAS 自己判断
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC)
	{
		return false;
	}

	// 地面：随时可以翻
	if (!ASC->HasMatchingGameplayTag(StatsTags::Ge_Stats_Fly))
	{
		return true;
	}

	// 飞行中：只有加速飞行（Ge.Stats.Fly.Fast）才能翻
	if (ASC->HasMatchingGameplayTag(StatsTags::Ge_Stats_Fly_Fast))
	{
		return true;
	}

	Debug::Print(TEXT("[GA_Dodge] 飞行中只有加速飞行（Ge.Stats.Fly.Fast）时才能闪避"),
				 Key_Blocked, 3.f, FColor::Yellow);
	return false;
}

bool UGA_Dodge::BuildDodgeContext()
{
	DodgeMontage = nullptr;
	DodgeDirection = DefaultDodgeDirection;
	DodgeWorldDirection = FVector::ForwardVector;
	bIsFlying = false;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return false;
	}

	// 飞行中：蒙太奇用飞行那套上/下/左/右
	bIsFlying = GetOwnerASC() && GetOwnerASC()->HasMatchingGameplayTag(StatsTags::Ge_Stats_Fly);

	// 参考坐标系：
	// - 默认用**角色自身**的前 / 右 / 上轴：角色朝左时往左按就是角色的前方（播 U），侧移才是 L / R。
	// - bUseCameraRelativeDirection 打开时改用相机的前 / 右轴（W→U、A→L）。
	const FRotator ViewRot = Character->GetControlRotation();
	const FRotator YawRot(0.f, ViewRot.Yaw, 0.f);

	const FVector RefForward = bUseCameraRelativeDirection
		                           ? (bIsFlying ? ViewRot.Vector().GetSafeNormal() : YawRot.Vector())
		                           : Character->GetActorForwardVector().GetSafeNormal();
	const FVector RefRight = bUseCameraRelativeDirection
		                         ? FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y)
		                         : Character->GetActorRightVector().GetSafeNormal();
	const FVector RefUp = Character->GetActorUpVector().GetSafeNormal();

	// 输入方向优先取「这一帧刚按下的输入」：
	// HandleMoveInput 在能力输入之前执行，已经把这一帧的方向写进 PendingMovementInputVector；
	// 而 CMC 的 Acceleration 还是上一次移动 tick 的值，会晚一帧 ——
	// 刚按方向就翻滚时，只看 Acceleration 会拿到上一帧的旧方向（表现就是「按左却往前翻」）。
	// 服务端上 Pending 通常是 0，会自动退回 Acceleration（也就是客户端上行还原的那份输入）。
	FVector InputDir = FVector::ZeroVector;
	if (const UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
	{
		InputDir = Character->GetPendingMovementInputVector();
		if (InputDir.IsNearlyZero())
		{
			InputDir = MoveComp->GetCurrentAcceleration();
		}
	}
	DebugDodgeAcceleration = InputDir.Size();
	if (!bIsFlying)
	{
		InputDir.Z = 0.f;
	}
	InputDir = InputDir.GetSafeNormal();

	// 飞行时「竖直输入」也参与选方向：抬头往上飞时往上是 U、往下是 D
	bool bVerticalInput = false;

	if (InputDir.IsNearlyZero())
	{
		// 没按方向键：用默认方向
		DodgeDirection = DefaultDodgeDirection;
	}
	else
	{
		const float ForwardDot = FVector::DotProduct(InputDir, RefForward);
		const float RightDot = FVector::DotProduct(InputDir, RefRight);
		const float UpDot = bIsFlying ? FVector::DotProduct(InputDir, RefUp) : 0.f;

		const float AbsForward = FMath::Abs(ForwardDot);
		const float AbsRight = FMath::Abs(RightDot);
		const float AbsUp = FMath::Abs(UpDot);

		// 三根轴里取最大的那根：斜着按只取主轴，这是四向闪避，不做八向
		if (AbsUp > AbsForward && AbsUp > AbsRight)
		{
			bVerticalInput = true;
			DodgeDirection = UpDot >= 0.f ? EDodgeDirection::Forward : EDodgeDirection::Backward;
		}
		else if (AbsForward >= AbsRight)
		{
			DodgeDirection = ForwardDot >= 0.f ? EDodgeDirection::Forward : EDodgeDirection::Backward;
		}
		else
		{
			DodgeDirection = RightDot >= 0.f ? EDodgeDirection::Right : EDodgeDirection::Left;
		}
	}

	// 位移方向跟着主轴走，避免「斜着按、蒙太奇却只有一个方向」的错位
	switch (DodgeDirection)
	{
	case EDodgeDirection::Forward:
		DodgeWorldDirection = bVerticalInput ? RefUp : RefForward;
		break;
	case EDodgeDirection::Backward:
		DodgeWorldDirection = bVerticalInput ? -RefUp : -RefForward;
		break;
	case EDodgeDirection::Left:
		DodgeWorldDirection = -RefRight;
		break;
	case EDodgeDirection::Right:
		DodgeWorldDirection = RefRight;
		break;
	}

	DodgeWorldDirection = DodgeWorldDirection.GetSafeNormal();

	return !DodgeWorldDirection.IsNearlyZero();
}

FGameplayTag UGA_Dodge::GetDodgeMontageTag() const
{
	const TMap<EDodgeDirection, FGameplayTag>& TagMap = bIsFlying ? FlyDodgeMontageTags : GroundDodgeMontageTags;

	if (const FGameplayTag* FoundTag = TagMap.Find(DodgeDirection))
	{
		if (FoundTag->IsValid())
		{
			return *FoundTag;
		}
	}

	return FGameplayTag();
}

void UGA_Dodge::FaceViewDirection()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	// 只对齐 Yaw：本项目的 Actor 俯仰恒为 0，抬头低头只在 Controller 上
	FRotator TargetRot = Character->GetActorRotation();
	TargetRot.Pitch = 0.f;
	TargetRot.Roll = 0.f;
	TargetRot.Yaw = Character->GetControlRotation().Yaw;

	Character->SetActorRotation(TargetRot);
}

void UGA_Dodge::SetRotationLocked(bool bLocked)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* MoveComp = Character ? Character->GetCharacterMovement() : nullptr;
	if (!MoveComp)
	{
		return;
	}

	if (bLocked)
	{
		if (bRotationLocked)
		{
			return;
		}
		bRotationLocked = true;
		MoveComp->bOrientRotationToMovement = false;
	}
	else
	{
		if (!bRotationLocked)
		{
			return;
		}
		bRotationLocked = false;

		// 如果此刻还在加速飞行，朝向归 GA_Fly_Fast 管（它把 bOrientRotationToMovement 关掉、让角色跟视角转）
		const bool bFastFlyActive =
			GetOwnerASC() && GetOwnerASC()->HasMatchingGameplayTag(StatsTags::Ge_Stats_Fly_Fast);
		MoveComp->bOrientRotationToMovement = !bFastFlyActive;
	}
}

void UGA_Dodge::PlayDodgeMontage()
{
	// AnimRootMotionTranslationScale：不要用动画自带的位移时传 0，只留表现。
	// 注意它只缩放「位移」，root motion 的旋转仍然生效。
	const float RootMotionTranslationScale = bUseMontageRootMotion ? 1.f : 0.f;

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, DodgeMontage,
			/*Rate=*/1.f,
			/*StartSection=*/NAME_None,
			/*bStopWhenAbilityEnds=*/true,
			RootMotionTranslationScale
		);

	if (!MontageTask)
	{
		bMontageFinished = true;
		return;
	}

	bMontageFinished = false;

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::HandleMontageFinished);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::HandleMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::HandleMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::HandleMontageCancelled);
	MontageTask->ReadyForActivation();
}

void UGA_Dodge::StartFallbackDash()
{
	const float Distance = GetEffectiveDodgeDistance();

	// 距离或时长为 0：只播动画，不推位移
	if (Distance <= UE_KINDA_SMALL_NUMBER || DodgeDuration <= UE_KINDA_SMALL_NUMBER)
	{
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Dodge] %s 的 DodgeDistance=%.1f / DodgeDuration=%.2f，本次闪避不位移"),
				*GetName(), DodgeDistance, DodgeDuration),
			Key_Dash, 8.f, FColor::Yellow);
		bDashFinished = true;
		return;
	}

	// 加算（而不是覆盖）速度：飞行中闪避不应该把飞行速度清掉，
	// 只是在原本速度上叠一段闪避方向的冲量，结束再把速度交给 CMC 的刹车。
	UAbilityTask_ApplyRootMotionConstantForce* DashTask =
		UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
			this,
			NAME_None,
			DodgeWorldDirection,
			/*Strength=*/Distance / DodgeDuration,
			DodgeDuration,
			/*bIsAdditive=*/true,
			/*StrengthOverTime=*/nullptr,
			ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
			FVector::ZeroVector,
			/*ClampVelocityOnFinish=*/0.f,
			/*bEnableGravity=*/false
		);

	if (!DashTask)
	{
		Debug::Print(
			FString::Printf(TEXT("[GA_Dodge] %s 创建位移 Task 失败"), *GetName()),
			Key_Dash, 8.f, FColor::Red);
		bDashFinished = true;
		return;
	}

	bDashFinished = false;

	DashTask->OnFinish.AddDynamic(this, &ThisClass::HandleDashFinished);
	DashTask->ReadyForActivation();
}

float UGA_Dodge::GetEffectiveDodgeDistance()
{
	float Distance = DodgeDistance;

	// 加速飞行时闪避得更远一点
	if (GetOwnerASC() && GetOwnerASC()->HasMatchingGameplayTag(StatsTags::Ge_Stats_Fly_Fast))
	{
		Distance *= SprintDodgeDistanceScale;
	}

	return Distance;
}

void UGA_Dodge::HandleMontageFinished()
{
	bMontageFinished = true;
	TryFinishAbility();
}

void UGA_Dodge::HandleMontageCancelled()
{
	// 被打断（比如被别的蒙太奇顶掉，或者这个蒙太奇本身播不起来）就别再等位移了，直接收尾
	Debug::Print(
		FString::Printf(
			TEXT("[GA_Dodge] %s 的蒙太奇被打断/没能播放，本次闪避提前结束"), *GetName()),
		Key_MontageCancelled, 8.f, FColor::Orange);

	bMontageFinished = true;
	bDashFinished = true;

	if (IsActive())
	{
		K2_EndAbility();
	}
}

void UGA_Dodge::HandleDashFinished()
{
	bDashFinished = true;

	// 位移结束就把朝向交还给 CMC
	if (bLockRotationDuringDodge)
	{
		SetRotationLocked(false);
	}

	TryFinishAbility();
}

void UGA_Dodge::TryFinishAbility()
{
	if (bMontageFinished && bDashFinished && IsActive())
	{
		K2_EndAbility();
	}
}
