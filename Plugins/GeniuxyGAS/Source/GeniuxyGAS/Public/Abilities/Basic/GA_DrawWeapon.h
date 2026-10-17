// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "Tags/GAMontageTags.h"
#include "GA_DrawWeapon.generated.h"

class UAbilityTask_WaitInputPress;
/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGA_DrawWeapon : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_DrawWeapon();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool PreLoadMontage() override;

	/**
	 * 能力的所有结束路径都会经过这里（正常结束、被取消、被别的能力打断……）。
	 * 收尾必须挂在这里，不能只挂 montage 的回调：
	 * 能力一旦 IsActive()==false，PlayMontageAndWait 的所有委托都会被静默丢弃
	 * （ShouldBroadcastAbilityTaskDelegates 要求 Ability->IsActive()）。
	 */
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

private:
	UFUNCTION()
	void HandleInputPress(float TimeWaited);

	/** 收剑 montage 的四个出口（Completed / BlendOut / Interrupted / Cancelled）都走这里 */
	UFUNCTION()
	void OnSheatheSwordEnd();

	/** 拔剑 montage 没能播出来时结束能力，避免能力卡在等输入的状态 */
	UFUNCTION()
	void OnDrawSwordCancelled();

	/** 重新武装"等待下一次按键"，旧任务先 EndTask */
	void ArmInputWait();

	/** 移除战斗姿态的两个 Infinite GE（幂等，重复调用安全） */
	void ClearStanceEffects();

	UPROPERTY(EditDefaultsOnly, meta=(Categories="Ge.Montage"))
	FGameplayTag DrawSwordMontageTag = GAMontageTags::Ge_Montage_DrawWeapon;
	
	UPROPERTY(EditDefaultsOnly, meta=(Categories="Ge.Montage"))
	FGameplayTag SheatheSwordMontageTag = GAMontageTags::Ge_Montage_SheatheWeapon;
	
	UPROPERTY()
	UAnimMontage* DrawSwordMontage = nullptr;
	
	UPROPERTY()
	UAnimMontage* SheatheSwordMontage = nullptr;

	UPROPERTY()
	UAbilityTask_WaitInputPress* WaitInputPress = nullptr;

	/**
	 * 用能力自己的状态机记"是否处于战斗姿态"，不要再靠 HasMatchingGameplayTag(InCombat)：
	 * 那两个 GE 是 AM_DrawSword 的 AnimNotify 在服务器上贴的，客户端/时序上都不可靠。
	 */
	bool bCombatStance = false;

	/** 防止 OnSheatheSwordEnd 和 EndAbility 重复移除 GE */
	bool bStanceCleared = true;
};
