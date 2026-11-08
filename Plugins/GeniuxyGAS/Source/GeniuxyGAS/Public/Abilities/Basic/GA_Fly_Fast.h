// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "Tags/MontageTags.h"
#include "GA_Fly_Fast.generated.h"

class UGameplayEffect;
class UAnimMontage;

/**
 * 加速飞行（按住型）：按下挂上 Ge.Stats.Fly.Fast 并加移速，松开结束。
 *
 * FastStart 表现由这里播一次，而不是交给 Fly Locomotion 的状态机。
 *
 * 两个坑（改这个能力时别踩）：
 * - PreLoadMontage() 必须**始终返回 true**：基类把 false 当成「不该激活」会直接
 *   K2_EndAbility()，加速飞行一结束 Ge.Stats.Fly.Fast 就没了。没配蒙太奇只该是「不播动画」。
 * - FastStart 的四个结束回调只打日志，**绝不能**连 K2_EndAbility：加速飞行由松开按键结束。
 *
 * 朝向：加速飞行期间由视角驱动，见 SetControllerYawRotation() 的注释。
 */
UCLASS()
class GENIUXYGAS_API UGA_Fly_Fast : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Fly_Fast();

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

	virtual bool PreLoadMontage() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> IncreaseMoveSpeedEffect;

	FActiveGameplayEffectHandle IncreaseMoveSpeedEffectHandle;

	UFUNCTION()
	void HandleInputRelease(float TimeWaited);

	/**
	 * 收到 Ge.Event.Land：撞地了，退出加速飞行。
	 * EndAbility 里会撤移速 GE、恢复朝向驱动。
	 */
	UFUNCTION()
	void HandleLandEvent(FGameplayEventData Payload);

	void PlayFastStartMontage();

	UFUNCTION()
	void HandleFastStartMontageEnded();

	/**
	 * 加速飞行期间让角色跟着视角朝向（朝哪看就往哪飞）。
	 *
	 * 三个开关是一组的，必须一起改：
	 * - `bUseControllerRotationYaw`：本地 PlayerController::UpdateRotation 每帧 FaceRotation，
	 *   最跟手；★但它只在本地 PlayerController 里调用，服务器不认这个开关。
	 * - `bUseControllerDesiredRotation`：让 CMC 的 PhysicsRotation 在**两端**都把角色转向
	 *   ControlRotation —— 服务器 / 模拟端才能跟上；否则会出现「本地转了、服务器没转」，
	 *   移动纠偏时把客户端拽回去，表现就是抖动 / 回弹。
	 * - `bOrientRotationToMovement` **必须关掉**：它每帧按移动方向再转一次，和上面两个打架，
	 *   这是本地抖动的主要原因。
	 */
	void SetControllerYawRotation(bool bEnable);

	/**********************************************************************/
	/*                              Montage                               */
	/**********************************************************************/
	/** 开始加速飞行时播放的蒙太奇 Tag（在 UGeAbilitySystemComponent 的 MontageMap 里查） */
	UPROPERTY(EditDefaultsOnly, Category = "Fly|Montage", meta = (Categories = "Ge.Montage"))
	FGameplayTag FastStartMontageTag = MontageTags::Ge_Montage_Fly_FastStart;

	/** 关掉就只加移速、不播 FastStart */
	UPROPERTY(EditDefaultsOnly, Category = "Fly|Montage")
	bool bPlayFastStartOnBeginFastFly = true;

	UPROPERTY()
	UAnimMontage* FastStartMontage = nullptr;
};
