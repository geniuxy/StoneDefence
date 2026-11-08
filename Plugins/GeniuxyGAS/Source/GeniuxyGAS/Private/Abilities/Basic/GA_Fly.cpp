// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Fly.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Animation/AnimMontage.h"
#include "GeniuxyDebugHelper.h"
#include "Tags/StatsTags.h"

UGA_Fly::UGA_Fly()
{
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Fly);
}

void UGA_Fly::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 基类在 PreLoadMontage() 失败时会结束能力，但它不会 return。
	// 这里的 PreLoadMontage 永远返回 true（见下），判活只是防御：
	// 能力真的已经结束时，不能再往一个死掉的能力上建 Task。
	if (!IsActive())
	{
		return;
	}

	if (bPlayHoverStartOnBeginFly && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		PlayHoverStartMontage();
	}

	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
	WaitInputPress->ReadyForActivation();
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
