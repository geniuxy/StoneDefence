// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Swim_Dive.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/AbilityTags.h"
#include "Tags/StatsTags.h"
#include "TimerManager.h"

UGA_Swim_Dive::UGA_Swim_Dive()
{
	// 只在水里能用：岸上 / 飞行时按 Ctrl 不会激活
	ActivationRequiredTags.AddTag(StatsTags::Ge_Stats_Swimming);

	// 「正在下潜」的状态信号：UGeLocomotionComponent 靠它决定要不要切中性浮力
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Swim_Dive);

	AbilityTags.AddTag(AbilityTags::Ge_Ability_Group_Swim);
}

void UGA_Swim_Dive::OnActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 立刻给一次，按住的第一帧就要有反应
	ApplyDiveInput();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DiveInputTimerHandle,
			this, &ThisClass::ApplyDiveInput,
			DiveInputInterval, /*bLoop=*/true);
	}

	// 松手结束
	UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::HandleInputRelease);
	WaitInputRelease->ReadyForActivation();
}

void UGA_Swim_Dive::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DiveInputTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Swim_Dive::ApplyDiveInput()
{
	if (!IsActive())
	{
		return;
	}

	// 出水了就收工，别在水外继续往下压
	UGeAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC || !ASC->HasMatchingGameplayTag(StatsTags::Ge_Stats_Swimming))
	{
		K2_EndAbility();
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		K2_EndAbility();
		return;
	}

	// 只有控制端才能加移动输入：它是输入、会随 saved move 复制到服务器，
	// 服务器那份（远程玩家的副本）再调一次就变成双份下潜速度。
	if (!Character->IsLocallyControlled())
	{
		return;
	}

	Character->AddMovementInput(FVector::DownVector, DiveInputScale);
}

void UGA_Swim_Dive::HandleInputRelease(float TimeWaited)
{
	K2_EndAbility();
}
