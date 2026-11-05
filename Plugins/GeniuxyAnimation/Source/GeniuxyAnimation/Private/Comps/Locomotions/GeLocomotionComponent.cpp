// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/Locomotions/GeLocomotionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Data/GeAnimationSetup.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/StatsTags.h"

UGeLocomotionComponent::UGeLocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UGeLocomotionComponent* UGeLocomotionComponent::Find(const AActor* InActor)
{
	if (!InActor) return nullptr;
	return InActor->FindComponentByClass<UGeLocomotionComponent>();
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

	BroadcastModeChanged(OldMode);
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
		OwnerMovementComp->SetMovementMode(MOVE_Walking);
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

	SwimTagHandle = OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_Swim).AddUObject(
		this, &ThisClass::HandleSwimTagChanged
	);

	ClimbTagHandle = OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_Climb).AddUObject(
		this, &ThisClass::HandleClimbTagChanged
	);

	CombatTagHandle = OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_InCombat).AddUObject(
		this, &ThisClass::HandleCombatTagChanged
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

	if (SwimTagHandle.IsValid())
	{
		OwnerASC->UnregisterGameplayTagEvent(SwimTagHandle, StatsTags::Ge_Stats_Swim);
		SwimTagHandle.Reset();
	}

	if (ClimbTagHandle.IsValid())
	{
		OwnerASC->UnregisterGameplayTagEvent(ClimbTagHandle, StatsTags::Ge_Stats_Climb);
		ClimbTagHandle.Reset();
	}

	if (CombatTagHandle.IsValid())
	{
		OwnerASC->UnregisterGameplayTagEvent(CombatTagHandle, StatsTags::Ge_Stats_InCombat);
		CombatTagHandle.Reset();
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

void UGeLocomotionComponent::HandleClimbTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bClimbTagActive = NewCount > 0;
	RefreshModeFromTags();
}

void UGeLocomotionComponent::HandleCombatTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bInCombat = NewCount > 0;
}
