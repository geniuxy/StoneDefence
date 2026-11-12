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
 * 出水时自己收尾：内部挂了一个 WaitGameplayTagRemoved(Ge.Stats.Swimming)，
 * 因为 GAS 的 ActivationRequiredTags 只判断「能否激活」，不会取消已经激活的能力。
 * 盯的必须是叶子 tag Ge.Stats.Swimming：父 tag（如 Ge.Stats.Swim）的计数会被子 tag 撑住，
 * 「计数变 0」的回调永远不触发。
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

	/** 游泳状态消失（出水 / 转飞行 / 死亡）→ 结束冲刺 */
	UFUNCTION()
	void HandleSwimTagRemoved();
};
