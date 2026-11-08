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
	/**
	 * 统一激活流程，**子类不要再覆盖这个函数**（已加 final）：
	 *   1) 派发蓝图事件 K2_ActivateAbility / K2_ActivateAbilityFromEvent
	 *   2) PrepareActivation()：子类的前置准备
	 *   3) PreLoadMontage()：失败 → 结束能力，**并且不再往下走**
	 *   4) OnActivateAbility()：子类真正的激活逻辑
	 *
	 * 因为失败分支在这里就 return 了，子类里不需要再写 `if (!IsActive()) return;` 这种判活代码。
	 */
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override final;

	/**
	 * 子类真正的激活逻辑：只有 PrepareActivation() 和 PreLoadMontage() 都成功才会被调到，
	 * 所以进来时能力一定是激活状态。
	 */
	virtual void OnActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData)
	{
	}

	/**
	 * 前置准备：在 PreLoadMontage() 之前执行。
	 *
	 * 给「蒙太奇依赖前面算出来的状态」的能力用（例如闪避要先算方向，蒙太奇是按方向取的）。
	 * 返回 false 表示这次不激活，基类会结束能力。
	 */
	virtual bool PrepareActivation()
	{
		return true;
	}

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
