// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "Tags/MontageTags.h"
#include "GA_Land.generated.h"

class UAnimMontage;

/**
 * 飞行落地（事件驱动，不需要输入键，放在 PassiveAbilities 里授予）。
 *
 * 触发链路：
 *   ASdCharacterBase::MoveBlockedBy（撞到可走地面 + 正在飞行 + 下坠速度够）
 *     → 发 Ge.Event.Land（EventMagnitude = 撞击瞬间的下坠速度）
 *     → 本能力靠 Triggers 接住，按速度分硬 / 软落地，播对应蒙太奇。
 *
 * 退出飞行**不在这里做**：GA_Fly / GA_Fly_Fast 各自监听同一个事件并结束自己，
 * Fly tag 一摘掉，UGeLocomotionComponent 就自动回到 Ground。
 *
 * 两个约定：
 * - 蒙太奇不带位移：播放时 AnimRootMotionTranslationScale = 0；
 * - 蒙太奇缺失只告警，不影响落地流程（飞行照样会退出）。
 */
UCLASS()
class GENIUXYGAS_API UGA_Land : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Land();

	virtual void OnActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool PreLoadMontage() override;

private:
	/** 硬落地判定：下坠速度够大，或者「水平速度快 + 俯冲角够陡」 */
	bool IsHardLanding(float FallSpeed, float Speed2D) const;

	void PlayLandingMontage();

	/** 蒙太奇播完 / 被打断：落地表现结束，能力收尾 */
	UFUNCTION()
	void HandleLandingMontageEnded();

	/**********************************************************************/
	/*                          Landing Detection                         */
	/**********************************************************************/
	/** 低于它当「贴地低飞」，不落地（角色侧也拦一次，这里是双保险） */
	UPROPERTY(EditDefaultsOnly, Category = "Land|Detect", meta = (ClampMin = "0"))
	float MinFallSpeed = 250.f;

	/** 硬落地：下坠速度阈值 */
	UPROPERTY(EditDefaultsOnly, Category = "Land|Detect", meta = (ClampMin = "0"))
	float HardFallSpeed = 900.f;

	/** 硬落地：水平速度阈值（配合俯冲角） */
	UPROPERTY(EditDefaultsOnly, Category = "Land|Detect", meta = (ClampMin = "0"))
	float HardFlySpeed2D = 1200.f;

	/** 硬落地：俯冲角阈值（撞击速度与水平面的夹角，单位度） */
	UPROPERTY(EditDefaultsOnly, Category = "Land|Detect", meta = (ClampMin = "0", ClampMax = "90"))
	float HardDiveAngleDeg = 50.f;

	/**********************************************************************/
	/*                              Montage                               */
	/**********************************************************************/
	/** 硬落地蒙太奇 Tag（想用哪一套自己在 AnimationSet 里指） */
	UPROPERTY(EditDefaultsOnly, Category = "Land|Montage", meta = (Categories = "Ge.Montage"))
	FGameplayTag HardLandMontageTag = MontageTags::Ge_Montage_Fly_Land_Hard;

	/** 软落地蒙太奇 Tag */
	UPROPERTY(EditDefaultsOnly, Category = "Land|Montage", meta = (Categories = "Ge.Montage"))
	FGameplayTag SoftLandMontageTag = MontageTags::Ge_Montage_Fly_Land_Soft;

	UPROPERTY()
	UAnimMontage* HardLandMontage = nullptr;

	UPROPERTY()
	UAnimMontage* SoftLandMontage = nullptr;

	/** 本次实际用的蒙太奇 */
	UPROPERTY()
	UAnimMontage* LandingMontage = nullptr;

	bool bHardLanding = false;
};
