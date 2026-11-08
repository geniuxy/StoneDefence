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
	if (!PreLoadMontage())
	{
		K2_EndAbility();
	}
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
