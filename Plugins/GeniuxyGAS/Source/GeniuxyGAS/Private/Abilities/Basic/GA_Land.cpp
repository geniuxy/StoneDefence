// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Land.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GeniuxyDebugHelper.h"
#include "Tags/EventTags.h"
#include "Tags/StatsTags.h"

UGA_Land::UGA_Land()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = EventTags::Ge_Event_FlyLand;
	AbilityTriggers.Add(TriggerData);

	// 落地期间挂 Landing：用来挡输入，同时挡住「落地中再落地」
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Landing);
	ActivationBlockedTags.AddTag(StatsTags::Ge_Stats_Landing);

	// 死了不播
	ActivationBlockedTags.AddTag(StatsTags::Ge_Stats_Dead);

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Land::OnActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		K2_EndAbility();
		return;
	}

	// 撞击瞬间的下坠速度由角色用 EventMagnitude 带过来：
	// 碰撞后 Velocity.Z 会被滑移修正掉，能力里再读就不准了。
	const float FallSpeed = TriggerEventData
		                        ? TriggerEventData->EventMagnitude
		                        : FMath::Max(0.f, -Character->GetVelocity().Z);
	const float Speed2D = Character->GetVelocity().Size2D();

	// 速度太低当「贴地低飞」（角色侧已经拦过一次，这里是双保险）
	if (FallSpeed < MinFallSpeed)
	{
		Debug::Print(
			FString::Printf(TEXT("[GA_Land] 下坠速度 %.1f < %.1f，当贴地，不落地"), FallSpeed, MinFallSpeed),
			"Land.Skip", 3.f, FColor::Silver);
		K2_EndAbility();
		return;
	}

	// 贴地那一下把竖直速度收掉，避免切 Walking 时被弹一下
	if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
	{
		MoveComp->Velocity.Z = FMath::Max(MoveComp->Velocity.Z, 0.f);
	}

	bHardLanding = IsHardLanding(FallSpeed, Speed2D);
	LandingMontage = bHardLanding ? HardLandMontage : SoftLandMontage;

	if (!LandingMontage)
	{
		Debug::Print(
			FString::Printf(TEXT("[GA_Land] %s落地没有可用蒙太奇，直接收尾"), bHardLanding ? TEXT("硬") : TEXT("软")),
			"Land.PreLoad", 5.f, FColor::Yellow);
		K2_EndAbility();
		return;
	}

	PlayLandingMontage();
}

bool UGA_Land::PreLoadMontage()
{
	HardLandMontage = nullptr;
	SoftLandMontage = nullptr;

	// 永远返回 true：基类把 false 当成「不该激活」会直接结束能力，
	// 而落地逻辑（以及退出飞行）不依赖动画资源，缺蒙太奇只该是「不播动画」。
	HardLandMontage = LoadMontageByTag(HardLandMontageTag);
	SoftLandMontage = LoadMontageByTag(SoftLandMontageTag);

	if (!HardLandMontage || !SoftLandMontage)
	{
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Land] %s 落地蒙太奇缺失：Hard(%s)=%s Soft(%s)=%s"),
				*GetName(),
				*HardLandMontageTag.ToString(), HardLandMontage ? TEXT("OK") : TEXT("NULL"),
				*SoftLandMontageTag.ToString(), SoftLandMontage ? TEXT("OK") : TEXT("NULL")),
			"Land.PreLoad", 8.f, FColor::Yellow);
	}

	return true;
}

bool UGA_Land::IsHardLanding(float FallSpeed, float Speed2D) const
{
	// 1) 砸得够狠：只看下坠速度
	if (FallSpeed >= HardFallSpeed)
	{
		return true;
	}

	// 2) 速度不够快、或者不够陡，都算软落地
	if (Speed2D < HardFlySpeed2D)
	{
		return false;
	}

	// 俯冲角用「撞击前的竖直速度 / 水平速度」算：撞击后 Velocity.Z 已经被修正过了
	const float DiveAngleDeg = FMath::RadiansToDegrees(
		FMath::Atan2(FallSpeed, FMath::Max(Speed2D, UE_KINDA_SMALL_NUMBER)));

	return DiveAngleDeg >= HardDiveAngleDeg;
}

void UGA_Land::PlayLandingMontage()
{
	// AnimRootMotionTranslationScale = 0：落地蒙太奇不带位移（位移交给物理/速度衰减）
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, LandingMontage,
			/*Rate=*/1.f,
			/*StartSection=*/NAME_None,
			/*bStopWhenAbilityEnds=*/true,
			/*AnimRootMotionTranslationScale=*/0.f
		);

	if (!MontageTask)
	{
		K2_EndAbility();
		return;
	}

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::HandleLandingMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::HandleLandingMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::HandleLandingMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::HandleLandingMontageEnded);
	MontageTask->ReadyForActivation();
}

void UGA_Land::HandleLandingMontageEnded()
{
	// 落地表现结束就收尾（被打断也直接收尾，不要把落地拖成常驻状态）
	if (IsActive())
	{
		K2_EndAbility();
	}
}
