// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "GA_Swim_Sprint.generated.h"

/**
 * 游泳冲刺（按住冲刺键）。
 *
 * 激活条件：带着 Ge.Stats.Swimming（在水里）。激活后挂 Ge.Stats.Swim.Fast，
 * 并叠一个 ×2 游泳速度的 GE；松手时结束并摘掉 GE。
 *
 * 出水时由 ASC 统一收尾：UGeAbilitySystemComponent 监听 Ge.Stats.Swimming 的移除，
 * 计数归零就 CancelAbilities(Ge.Ability.Group.Swim)，本能力不用再自己挂
 * UAbilityTask_WaitGameplayTagRemoved（那个方案只保一条能力，加新能力就得重挂一遍）。
 */
UCLASS()
class GENIUXYGAS_API UGA_Swim_Sprint : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Swim_Sprint();

	virtual void OnActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> IncreaseMoveSpeedEffect;

	FActiveGameplayEffectHandle IncreaseMoveSpeedEffectHandle;

	UFUNCTION()
	void HandleInputRelease(float TimeWaited);
};
