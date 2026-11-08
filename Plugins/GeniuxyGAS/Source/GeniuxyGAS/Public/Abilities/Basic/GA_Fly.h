// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "Tags/MontageTags.h"
#include "GA_Fly.generated.h"

class UAnimMontage;
/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGA_Fly : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Fly();

	virtual void OnActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool PreLoadMontage() override;

private:
	UFUNCTION()
	void HandleInputPress(float TimeWaited);

	/**
	 * 收到 Ge.Event.Land：撞地了，退出飞行。
	 * 结束方式就是 K2_EndAbility —— Fly tag 一摘掉，LocomotionComp 自动回 Ground。
	 */
	UFUNCTION()
	void HandleLandEvent(FGameplayEventData Payload);

	void PlayHoverStartMontage();

	UFUNCTION()
	void HandleHoverStartMontageEnded();

	/**********************************************************************/
	/*                              Montage                               */
	/**********************************************************************/

	/** 进飞行状态时是否播 HoverStart */
	UPROPERTY(EditDefaultsOnly, Category = "Fly|Montage")
	bool bPlayHoverStartOnBeginFly = true;

	/** 起飞时播放的蒙太奇 Tag（在 UGeAbilitySystemComponent 的 MontageMap 里查） */
	UPROPERTY(EditDefaultsOnly, Category = "Fly|Montage", meta = (Categories = "Ge.Montage"))
	FGameplayTag HoverStartMontageTag = MontageTags::Ge_Montage_Fly_HoverStart;

	UPROPERTY()
	UAnimMontage* HoverStartMontage = nullptr;
};
