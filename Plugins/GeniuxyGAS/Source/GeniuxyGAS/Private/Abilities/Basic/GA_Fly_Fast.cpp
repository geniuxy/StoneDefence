// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Fly_Fast.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Animation/AnimMontage.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GeniuxyDebugHelper.h"
#include "Tags/StatsTags.h"

UGA_Fly_Fast::UGA_Fly_Fast()
{
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Fly_Fast);
	ActivationRequiredTags.AddTag(StatsTags::Ge_Stats_Fly);
}

void UGA_Fly_Fast::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!IsActive())
	{
		return;
	}

	if (!IncreaseMoveSpeedEffect)
	{
		K2_EndAbility();
		return;
	}

	// 加速飞行期间：朝向由视角驱动（朝哪看就往哪飞）
	SetControllerYawRotation(true);

	// 开始加速飞行的表现只在这里播一次，ABP 里 Fly 状态机的 FastStart 要去掉
	if (bPlayFastStartOnBeginFastFly && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		PlayFastStartMontage();
	}

	if (K2_HasAuthority())
	{
		IncreaseMoveSpeedEffectHandle = BP_ApplyGameplayEffectToOwner(
			IncreaseMoveSpeedEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		);
	}

	UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::HandleInputRelease);
	WaitInputRelease->ReadyForActivation();
}

void UGA_Fly_Fast::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	SetControllerYawRotation(false);

	if (GetOwnerASC() && IncreaseMoveSpeedEffectHandle.IsValid())
	{
		GetOwnerASC()->RemoveActiveGameplayEffect(IncreaseMoveSpeedEffectHandle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_Fly_Fast::PreLoadMontage()
{
	FastStartMontage = nullptr;

	// 没配 FastStart 只是「不播动画」。
	if (!FastStartMontageTag.IsValid())
	{
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Fly_Fast] %s 没有配置 FastStart 蒙太奇 Tag，开始加速飞行时不会播 FastStart"),
				*GetName()),
			"PreLoad", 8.f, FColor::Yellow);
		return true;
	}

	FastStartMontage = LoadMontageByTag(FastStartMontageTag);
	if (!FastStartMontage)
	{
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Fly_Fast] %s 取不到 Tag %s 对应的蒙太奇（AnimationSet 没配或加载失败）"),
				*GetName(), *FastStartMontageTag.ToString()),
			"PreLoad", 8.f, FColor::Yellow);
	}

	return true;
}

void UGA_Fly_Fast::PlayFastStartMontage()
{
	if (!FastStartMontage)
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FastStartMontage);

	if (!MontageTask)
	{
		return;
	}

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::HandleFastStartMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::HandleFastStartMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::HandleFastStartMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::HandleFastStartMontageEnded);
	MontageTask->ReadyForActivation();
}

void UGA_Fly_Fast::HandleFastStartMontageEnded()
{
	Debug::Print(
		TEXT("[GA_Fly_Fast] FastStart 蒙太奇播放结束，交给快速飞行循环姿态接管"), "FastStartEnd", 3.f
	);
}

void UGA_Fly_Fast::SetControllerYawRotation(bool bEnable)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!OwnerCharacter)
	{
		return;
	}

	// 本地：PlayerController::UpdateRotation 每帧调 FaceRotation，把角色直接摆到相机 yaw 上，最跟手
	OwnerCharacter->bUseControllerRotationYaw = bEnable;

	if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
	{
		// 服务器不认 bUseControllerRotationYaw（FaceRotation 只在本地 PlayerController 里调），
		// 服务端 bUseControllerDesiredRotation 让 CMC 在两端都朝 ControlRotation 转。
		MoveComp->bUseControllerDesiredRotation = bEnable;

		// 此时如果FlyFast中，则bOrientRotationToMovement=false;
		// 如果不在FlyFast中，且还在Dodging中，bOrientRotationToMovement=false;(交由Dodging的EndAbility去还原bOrientRotationToMovement)
		// 如果不在FlyFast中，且不在Dodging中，bOrientRotationToMovement=true;
		const bool bKeepOrientationLocked =
			!bEnable && GetOwnerASC() && GetOwnerASC()->HasMatchingGameplayTag(StatsTags::Ge_Stats_Dodging);

		MoveComp->bOrientRotationToMovement = !bEnable && !bKeepOrientationLocked;
	}
}

void UGA_Fly_Fast::HandleInputRelease(float TimeWaited)
{
	K2_EndAbility();
}
