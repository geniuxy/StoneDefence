// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Fly.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Animation/AnimMontage.h"
#include "GeniuxyDebugHelper.h"
#include "Tags/EventTags.h"
#include "Tags/StatsTags.h"

UGA_Fly::UGA_Fly()
{
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Fly);
}

void UGA_Fly::OnActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (bPlayHoverStartOnBeginFly && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		PlayHoverStartMontage();
	}

	// 撞地（Ge.Event.Land）就退出飞行：和 GA_Land 监听同一个事件，各管各的——
	// 它播落地蒙太奇，我们负责结束飞行。
	UAbilityTask_WaitGameplayEvent* WaitLandEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, EventTags::Ge_Event_FlyLand, /*OptionalExternalTarget=*/nullptr,
		/*OnlyTriggerOnce=*/true, /*OnlyMatchExact=*/true);
	WaitLandEvent->EventReceived.AddDynamic(this, &ThisClass::HandleLandEvent);
	WaitLandEvent->ReadyForActivation();

	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_Fly::HandleLandEvent(FGameplayEventData Payload)
{
	Debug::Print(TEXT("[GA_Fly] 收到 Ge.Event.Land，退出飞行"),
	             TEXT("Fly.Land"), 3.f, FColor::Orange);

	K2_EndAbility();
}

bool UGA_Fly::PreLoadMontage()
{
	HoverStartMontage = nullptr;

	// 没配 HoverStart 只是「不播动画」。
	if (!HoverStartMontageTag.IsValid())
	{
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Fly] %s 没有配置 HoverStart 蒙太奇 Tag，起飞时不会播 HoverStart"),
				*GetName()),
			"PreLoad", 8.f, FColor::Yellow);
		return true;
	}

	HoverStartMontage = LoadMontageByTag(HoverStartMontageTag);
	if (!HoverStartMontage)
	{
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Fly] %s 取不到 Tag %s 对应的蒙太奇（AnimationSet 没配或加载失败），起飞时不会播 HoverStart"),
				*GetName(), *HoverStartMontageTag.ToString()),
			"PreLoad", 8.f, FColor::Yellow);
	}

	return true;
}

void UGA_Fly::PlayHoverStartMontage()
{
	if (!HoverStartMontage)
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HoverStartMontage);

	if (!MontageTask)
	{
		return;
	}

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::HandleHoverStartMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::HandleHoverStartMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::HandleHoverStartMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::HandleHoverStartMontageEnded);
	MontageTask->ReadyForActivation();
}

void UGA_Fly::HandleHoverStartMontageEnded()
{
	Debug::Print(
		TEXT("[GA_Fly] HoverStart 蒙太奇播放结束，交给悬停姿态接管"), "HoverStartEnd", 3.f
	);
}

void UGA_Fly::HandleInputPress(float TimeWaited)
{
	K2_EndAbility();
}
