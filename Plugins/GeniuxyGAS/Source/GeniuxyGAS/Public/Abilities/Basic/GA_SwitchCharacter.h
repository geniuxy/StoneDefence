// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "GameplayTagContainer.h"
#include "GeniuxyGASType.h"
#include "GA_SwitchCharacter.generated.h"

/**
 * 切换当前出战角色
 */
UCLASS()
class GENIUXYGAS_API UGA_SwitchCharacter : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_SwitchCharacter();

	virtual void OnActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	FGameplayTag SwitchCharacterEventTag;

	/**
	 * "第 1 个队友"对应的 InputKey。
	 *
	 * 默认 AbilityOne：键盘 1 键 -> 槽位 1，键盘 2 键 -> 槽位 2……
	 * 注意这里填的是 EAbilityInputID 的枚举项，不是键盘数字。
	 */
	UPROPERTY(EditDefaultsOnly, Category="Switch Character")
	EAbilityInputID FirstSlotInputKey = EAbilityInputID::AbilityOne;
};
