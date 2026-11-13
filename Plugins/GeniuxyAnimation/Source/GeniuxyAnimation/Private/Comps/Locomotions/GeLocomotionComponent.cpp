// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/Locomotions/GeLocomotionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Data/GeAnimationSetup.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GeniuxyDebugHelper.h"
#include "Tags/StatsTags.h"

UGeLocomotionComponent::UGeLocomotionComponent()
{
	// 需要 Tick 来按浸没深度调浮力，但只在 Swim 模式期间真正开启（见 ChangeModeInternal），
	// 其它模式下 tick 是关掉的，不占帧。
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UGeLocomotionComponent* UGeLocomotionComponent::Find(const AActor* InActor)
{
	if (!InActor) return nullptr;
	return InActor->FindComponentByClass<UGeLocomotionComponent>();
}

void UGeLocomotionComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateSwimBuoyancy();
	UpdateSwimHorizontalBrake(DeltaTime);
}

EGeLocomotionMode UGeLocomotionComponent::GetModeFromActor(const AActor* InActor)
{
	const UGeLocomotionComponent* Comp = Find(InActor);
	return Comp ? Comp->GetMode() : EGeLocomotionMode::Ground;
}

void UGeLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

	// 配置里指定了脚部 IK 开关就以它为准（BeginPlay 时就同步一次，
	// 不用等角色主动调 SetAnimationSetup）。
	if (AnimationSetup)
	{
		bFootIKEnabled = AnimationSetup->bCanFootIK;
	}

	BindAbilitySystemTags();
}

void UGeLocomotionComponent::SetAnimationSetup(UGeAnimationSetup* InSetup)
{
	if (AnimationSetup == InSetup) return;

	AnimationSetup = InSetup;

	// 手工配置的唯一入口：同步脚部 IK 开关
	if (AnimationSetup)
	{
		bFootIKEnabled = AnimationSetup->bCanFootIK;
	}

	// 通知角色：层实现类可能变了，让 AnimInstance 重新 LinkAnimClassLayers
	OnAnimationSetupChanged.Broadcast();
}

void UGeLocomotionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindAbilitySystemTags();

	Super::EndPlay(EndPlayReason);
}

bool UGeLocomotionComponent::ShouldDriveMovement() const
{
	if (!OwnerCharacter) return false;

	// 模拟端（SimulatedProxy）不主动改 MovementMode，只跟随服务器复制过来的结果，
	// 否则客户端和服务器会互相覆盖，出现「本地看着在飞、服务器认为在走」的抖动。
	return OwnerCharacter->HasAuthority() || OwnerCharacter->IsLocallyControlled();
}

/**********************************************************************/
/*                               Mode                                 */
/**********************************************************************/

void UGeLocomotionComponent::SetMode(EGeLocomotionMode InNewMode)
{
	ChangeModeInternal(InNewMode, /*bPushHistory=*/false);
}

void UGeLocomotionComponent::PushMode(EGeLocomotionMode InNewMode)
{
	ChangeModeInternal(InNewMode, /*bPushHistory=*/true);
}

void UGeLocomotionComponent::PopMode()
{
	if (ModeHistory.Num() == 0)
	{
		SetMode(EGeLocomotionMode::Ground);
		return;
	}

	const EGeLocomotionMode RestoredMode = ModeHistory.Pop();
	ChangeModeInternal(RestoredMode, /*bPushHistory=*/false);
}

void UGeLocomotionComponent::ChangeModeInternal(EGeLocomotionMode InNewMode, bool bPushHistory)
{
	if (Mode == InNewMode) return;

	const EGeLocomotionMode OldMode = Mode;

	if (bPushHistory)
	{
		ModeHistory.Push(Mode);
	}

	PreviousMode = Mode;
	Mode = InNewMode;

	if (ShouldDriveMovement())
	{
		ApplyMovementMode(Mode);
	}

	// 游泳期间要每帧按浸没深度调浮力（见 UpdateSwimBuoyancy），其它模式不需要。
	// 这里对两端都生效：浮力是 CMC 的本地属性，两端必须算成一样，否则位置会互相拉扯。
	if (Mode == EGeLocomotionMode::Swim)
	{
		SetComponentTickEnabled(true);
	}
	else
	{
		SetComponentTickEnabled(false);
		RestoreSurfaceBuoyancy();
		bSwimUnderwater = false;
	}

	BroadcastModeChanged(OldMode);
}

void UGeLocomotionComponent::UpdateSwimBuoyancy()
{
	if (!OwnerMovementComp || !OwnerMovementComp->IsSwimming())
	{
		return;
	}

	// 引擎的浸没比例：0 = 刚碰到水，1 = 完全没入
	const float Depth = OwnerMovementComp->ImmersionDepth();
	bSwimUnderwater = Depth >= SwimUnderwaterDepth;

	// 水面稳定带：一旦浮到这个深度以上，就禁止继续上升，并轻轻压回带内。
	// 不这样做的话，浮力平衡点（Depth ≈ 0.51）几乎压在体积顶面（Depth = 0.5）上，
	// 胶囊中心一越界就会「出水摘 tag + 引擎切 MOVE_Falling」，落回来再入水，游泳状态闪一下。
	// 只在驱动移动的端上改速度（服务器 / 本地控制端），模拟端跟随复制。
	if (ShouldDriveMovement() && Depth < SwimSurfaceKeepDepth)
	{
		OwnerMovementComp->Velocity.Z = FMath::Min(OwnerMovementComp->Velocity.Z, -SwimSurfacePushDownSpeed);
	}

	// 在按「下潜」：GA_Swim_Dive 挂的 tag。有了它，从水面往下潜的整个过程都是中性浮力，
	const bool bDiveRequested = OwnerASC && OwnerASC->HasMatchingGameplayTag(StatsTags::Ge_Stats_Swim_Dive);

	// 水面附近且没在下潜 → 交回角色原本的浮力（浮在水面）
	if (!bSwimUnderwater && !bDiveRequested)
	{
		RestoreSurfaceBuoyancy();
		return;
	}

	// 需要「停在水下」：先把角色原本的浮力存下来，再换成中性。
	// 用懒缓存而不是进水时缓存，是因为这个值由角色在 BeginPlay 里设置，
	// 这样无论谁先跑，抓到的都是角色当时的真实配置。
	if (!bSwimBuoyancyCached)
	{
		CachedSurfaceBuoyancy = OwnerMovementComp->Buoyancy;
		bSwimBuoyancyCached = true;
	}

	OwnerMovementComp->Buoyancy = SwimUnderwaterBuoyancy;
}

void UGeLocomotionComponent::UpdateSwimHorizontalBrake(float DeltaTime)
{
	if (!OwnerMovementComp || !OwnerMovementComp->IsSwimming() || SwimHorizontalBrakeDecel <= 0.f)
	{
		SwimNoInputTime = 0.f;
		return;
	}

	// 只在会驱动移动的端上改速度（服务器 / 本地控制端），模拟端跟随复制
	if (!ShouldDriveMovement())
	{
		return;
	}

	// 判断「有没有移动输入」用 CMC 自己算出来的 Acceleration，**不要**用
	// GetPendingMovementInputVector / GetLastMovementInputVector：
	// 那两个由 CMC 在它自己的 tick 里消费，和本组件的 tick 先后顺序没有保证。
	// 客户端上一旦跑在「已被消费成 0」的时机，就会每帧把速度刹掉 ——
	// 表现就是客户端里人在水里原地不动、怎么按都游不起来。
	// Acceleration 是 PerformMovement 里按输入算出来的值，两种顺序下都会保留一帧，
	// 用它判定能免疫这个 tick 顺序问题（注意成员是 protected 的，要用 GetCurrentAcceleration()）。
	const bool bHasInput = OwnerMovementComp->GetCurrentAcceleration().SizeSquared() > KINDA_SMALL_NUMBER;

	// 再给一点迟滞：连续一段没有输入才刹，避免单帧空档（输入相位 / 网络）误刹
	SwimNoInputTime = bHasInput ? 0.f : (SwimNoInputTime + DeltaTime);
	if (SwimNoInputTime < SwimBrakeNoInputDelay)
	{
		return;
	}

	// 只吃水平分量：竖直留给浮力（上浮）和下潜，别把人钉在水里
	const float Speed2D = FVector(OwnerMovementComp->Velocity.X, OwnerMovementComp->Velocity.Y, 0.f).Size();
	if (Speed2D <= 0.f)
	{
		return;
	}

	const float NewSpeed2D = FMath::Max(0.f, Speed2D - SwimHorizontalBrakeDecel * DeltaTime);
	const float Scale = NewSpeed2D / Speed2D;
	OwnerMovementComp->Velocity.X *= Scale;
	OwnerMovementComp->Velocity.Y *= Scale;

	if (bDebugSwimBrake && OwnerCharacter && OwnerCharacter->IsLocallyControlled())
	{
		Debug::Print(
			FString::Printf(
				TEXT("[SwimBrake] 刹水平  Vel2D=%.0f  Accel=%.0f  NoInput=%.2fs"),
				Speed2D,
				OwnerMovementComp->GetCurrentAcceleration().Size(),
				SwimNoInputTime),
			TEXT("Swim.Brake"), 0.f, FColor::Cyan);
	}
}

void UGeLocomotionComponent::RestoreSurfaceBuoyancy()
{
	if (!bSwimBuoyancyCached)
	{
		return;
	}

	bSwimBuoyancyCached = false;

	if (OwnerMovementComp)
	{
		OwnerMovementComp->Buoyancy = CachedSurfaceBuoyancy;
	}
}

void UGeLocomotionComponent::BroadcastModeChanged(EGeLocomotionMode InOldMode)
{
	OnModeChanged.Broadcast(InOldMode, Mode);
	OnModeChangedBP.Broadcast(InOldMode, Mode);
}

void UGeLocomotionComponent::RefreshModeFromTags()
{
	// 优先级：攀爬 > 飞行 > 游泳 > 地面。谁在生效就以谁为准。
	EGeLocomotionMode DesiredMode = EGeLocomotionMode::Ground;
	if (bClimbTagActive)
	{
		DesiredMode = EGeLocomotionMode::Climb;
	}
	else if (bFlyTagActive)
	{
		DesiredMode = EGeLocomotionMode::Fly;
	}
	else if (bSwimTagActive)
	{
		DesiredMode = EGeLocomotionMode::Swim;
	}

	SetMode(DesiredMode);
}

void UGeLocomotionComponent::ApplyMovementMode_Implementation(EGeLocomotionMode InMode)
{
	if (!OwnerMovementComp) return;

	switch (InMode)
	{
	case EGeLocomotionMode::Ground:
		// 这里**不能**无条件 SetMovementMode(MOVE_Walking)。
		//
		// 典型场景是出水：引擎在 PhysSwimming 末尾发现已经不在水体体积里，会自己把人切到
		// MOVE_Falling（CharacterMovementComponent.cpp 里 `!GetPhysicsVolume()->bWaterVolume
		// && IsSwimming()` 那一段）。如果我们在「摘掉 Swim tag → Mode 回 Ground」时再硬切
		// Walking，人就会在水面上「空中站立」一帧，出水手感直接崩。
		//
		// 所以分开处理：
		// - 已经在地上（Walking / NavWalking）：什么都不用改，蹲 / 爬这类细节由引擎维护；
		// - 在空中（刚从飞行 / 游泳 / 自定义模式退出来）：交给重力，落地由引擎的
		//   SetPostLandedPhysics 负责（它还会判断落点是水就切回 Swimming）。
		if (!OwnerMovementComp->IsMovingOnGround() && !OwnerMovementComp->IsFalling())
		{
			OwnerMovementComp->SetMovementMode(MOVE_Falling);
		}
		break;

	case EGeLocomotionMode::Fly:
		OwnerMovementComp->SetMovementMode(MOVE_Flying);
		break;

	case EGeLocomotionMode::Swim:
		OwnerMovementComp->SetMovementMode(MOVE_Swimming);
		break;

	case EGeLocomotionMode::Climb:
		// 默认用 Flying 承载攀爬：不受重力、速度可控、无需自定义移动组件。
		// 如果你们之后写了自定义 UCharacterMovementComponent，把这里改成
		// OwnerMovementComp->SetMovementMode(MOVE_Custom, CMOVE_Climb) 即可。
		OwnerMovementComp->SetMovementMode(MOVE_Flying);
		break;

	default:
		break;
	}
}

/**********************************************************************/
/*                            GAS Tags                                */
/**********************************************************************/

void UGeLocomotionComponent::BindAbilitySystemTags()
{
	if (!OwnerASC) return;

	FlyTagHandle = OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_Fly).AddUObject(
		this, &ThisClass::HandleFlyTagChanged
	);
	FlyFastTagHandle = OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_Fly_Fast).AddUObject(
		this, &ThisClass::HandleFlyFastTagChanged
	);

	SwimTagHandle = OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_Swimming).AddUObject(
		this, &ThisClass::HandleSwimTagChanged
	);
	SwimFastTagHandle = OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_Swim_Fast).AddUObject(
		this, &ThisClass::HandleSwimFastTagChanged
	);

	ClimbTagHandle = OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_Climb).AddUObject(
		this, &ThisClass::HandleClimbTagChanged
	);
}

void UGeLocomotionComponent::UnbindAbilitySystemTags()
{
	if (!OwnerASC) return;

	if (FlyTagHandle.IsValid())
	{
		OwnerASC->UnregisterGameplayTagEvent(FlyTagHandle, StatsTags::Ge_Stats_Fly);
		FlyTagHandle.Reset();
	}
	if (FlyFastTagHandle.IsValid())
	{
		OwnerASC->UnregisterGameplayTagEvent(FlyFastTagHandle, StatsTags::Ge_Stats_Fly_Fast);
		FlyFastTagHandle.Reset();
	}

	if (SwimTagHandle.IsValid())
	{
		OwnerASC->UnregisterGameplayTagEvent(SwimTagHandle, StatsTags::Ge_Stats_Swimming);
		SwimTagHandle.Reset();
	}
	if (SwimFastTagHandle.IsValid())
	{
		OwnerASC->UnregisterGameplayTagEvent(SwimFastTagHandle, StatsTags::Ge_Stats_Swim_Fast);
		SwimFastTagHandle.Reset();
	}

	if (ClimbTagHandle.IsValid())
	{
		OwnerASC->UnregisterGameplayTagEvent(ClimbTagHandle, StatsTags::Ge_Stats_Climb);
		ClimbTagHandle.Reset();
	}
}

void UGeLocomotionComponent::HandleFlyTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bFlyTagActive = NewCount > 0;
	RefreshModeFromTags();
}

void UGeLocomotionComponent::HandleFlyFastTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bFlyFastTagActive = NewCount > 0;
}

void UGeLocomotionComponent::HandleSwimTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bSwimTagActive = NewCount > 0;
	RefreshModeFromTags();
}

void UGeLocomotionComponent::HandleSwimFastTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bSwimFastTagActive = NewCount > 0;
}

void UGeLocomotionComponent::HandleClimbTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bClimbTagActive = NewCount > 0;
	RefreshModeFromTags();
}
