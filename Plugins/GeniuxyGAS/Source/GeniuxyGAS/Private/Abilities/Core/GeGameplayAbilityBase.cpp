// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Core/GeGameplayAbilityBase.h"

#include "Animation/AnimMontage.h"
#include "Comps/GeAbilitySystemComponent.h"

void UGeGameplayAbilityBase::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 蓝图事件（K2_ActivateAbility / K2_ActivateAbilityFromEvent）由引擎基类派发
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 1) 子类前置准备（例如闪避要先算方向，因为蒙太奇按方向取）
	if (!PrepareActivation())
	{
		K2_EndAbility();
		return;
	}

	// 2) 预加载蒙太奇：失败就结束，并且**在这里 return**。
	//    子类的 OnActivateAbility() 不会被调用，所以子类不用再判活。
	if (!PreLoadMontage())
	{
		K2_EndAbility();
		return;
	}

	// 3) 子类真正的激活逻辑
	OnActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

UGeAbilitySystemComponent* UGeGameplayAbilityBase::GetOwnerASC()
{
	if (!OwnerASC)
	{
		OwnerASC = Cast<UGeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	}

	return OwnerASC;
}

TSoftObjectPtr<UAnimMontage> UGeGameplayAbilityBase::GetMontageByTag(FGameplayTag InTag)
{
	if (!GetOwnerASC()) return nullptr;

	const auto* FoundPtr = OwnerASC->GetMontageMap().Find(InTag);
	if (!FoundPtr)
	{
		return nullptr;
	}
	return *FoundPtr;
}

UAnimMontage* UGeGameplayAbilityBase::LoadMontageByTag(FGameplayTag InTag)
{
	if (!InTag.IsValid())
	{
		return nullptr;
	}

	const TSoftObjectPtr<UAnimMontage> SoftMontage = GetMontageByTag(InTag);
	if (SoftMontage.IsNull())
	{
		return nullptr;
	}

	// 已经在内存里就直接用，否则同步读一次（不要依赖预加载标记）
	if (UAnimMontage* LoadedMontage = SoftMontage.Get())
	{
		return LoadedMontage;
	}

	return SoftMontage.LoadSynchronous();
}
