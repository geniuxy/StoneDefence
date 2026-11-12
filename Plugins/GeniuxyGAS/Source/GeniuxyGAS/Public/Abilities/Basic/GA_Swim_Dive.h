// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "GA_Swim_Dive.generated.h"

/**
 * 游泳下潜（按住 Ctrl）。
 *
 * 本能力只做两件事：
 * 1. 激活期间挂 Ge.Stats.Swim.Dive —— 这是「正在下潜」的状态信号；
 * 2. 每 DiveInputInterval 秒给角色加一次向下的移动输入，按住就一直潜。
 *
 * 浮力不在这里改：UGeLocomotionComponent 会读 Ge.Stats.Swim.Dive + 角色当前浸没深度
 * 来切换浮力（水下中性、水面恢复），所以「松开 Ctrl 停在水下、按空格才浮上去」是那层的职责。
 * 这样能力结束（松手）之后浮力状态也不会丢。
 *
 * 为什么用 AddMovementInput 而不是直接改 Velocity.Z：
 * AddMovementInput 是「输入」，会随 saved move 复制给服务器；直接改速度只有本端生效，联机会各游各的。
 * 而它只能由控制端调用（本地玩家 / 服务器上的 AI），所以这里用 IsLocallyControlled()
 * 挡住「服务器上的远程玩家副本」，避免同一个人被加两遍向下输入。
 *
 * 激活条件：必须带着 Ge.Stats.Swim（在游泳里）。
 *
 * 数据侧还要配两处（不在 C++ 里）：
 * 1. PA_AbilitySystemGenerics → GameplayAbilityInputActions 里给 Ctrl 绑一个 InputAction，IMC 里也绑；
 * 2. 能力表（角色自身的 Abilities/BasicAbilities，或 PA 的 CommonAbilities）加一条：
 *    AbilityClass = GA_Swim_Dive，InputID = Ctrl，Level = 1。
 */
UCLASS()
class GENIUXYGAS_API UGA_Swim_Dive : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Swim_Dive();

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
	/** 定时器回调：往角色加一次向下移动输入 */
	UFUNCTION()
	void ApplyDiveInput();

	UFUNCTION()
	void HandleInputRelease(float TimeWaited);

	/** 下潜输入强度（0~1，乘在向下方向上的比例） */
	UPROPERTY(EditDefaultsOnly, Category = "Swim", meta = (ClampMin = "0.0", UIMax = "1.0"))
	float DiveInputScale = 1.f;

	/** 下潜输入的施加间隔（秒）：太大会一顿一顿，太小没意义 */
	UPROPERTY(EditDefaultsOnly, Category = "Swim", meta = (ClampMin = "0.01", UIMax = "0.1"))
	float DiveInputInterval = 0.02f;

	FTimerHandle DiveInputTimerHandle;
};
