// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GeGameplayAbilityBase.generated.h"

class UGeAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGeGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	UGeAbilitySystemComponent* GetOwnerASC();

	UPROPERTY()
	UGeAbilitySystemComponent* OwnerASC;

	TSoftObjectPtr<UAnimMontage> GetMontageByTag(FGameplayTag InTag);

	/**
	 * 按 Tag 从角色的蒙太奇表里取一个**确保已加载**的蒙太奇。
	 *
	 * 不要用 ASC 的 AreMontagesLoaded() 当保险
	 * 这里统一走 Get() -> 拿不到再 LoadSynchronous()。
	 *
	 * @return Tag 无效 / 表里没有 / 加载失败都返回 nullptr
	 */
	UAnimMontage* LoadMontageByTag(FGameplayTag InTag);

	virtual bool PreLoadMontage() { return true; }
};
