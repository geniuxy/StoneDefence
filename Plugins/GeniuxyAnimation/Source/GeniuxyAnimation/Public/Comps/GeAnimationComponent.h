// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GeniuxyAnimationType.h"
#include "Components/ActorComponent.h"
#include "GeAnimationComponent.generated.h"

class UCharacterAnimationSet;
class UGeAnimationSetup;

/** 动画配置变化（非同动态委托，C++ 侧用） */
DECLARE_MULTICAST_DELEGATE(FOnGeAnimationSetupChanged);

/**
 * 角色的动画组件：持有「动画数据」，并提供蒙太奇播放入口。
 *
 * 它管两块东西：
 * 1. 蒙太奇      —— UCharacterAnimationSet，按 Tag 播一次性动作（消费方：能力 / 玩法）；
 * 2. 动画配置    —— UGeAnimationSetup，「用哪套移动层 + 要不要脚部 IK」（消费方：AnimInstance）。
 *
 * 为什么动画配置放这里，而不放 UGeLocomotionComponent：
 * 移动组件持有的是**运行时状态**（现在在 Ground 还是 Fly），动画配置是**设计期资产**；
 * 按领域归类，设计期资产属于动画组件。读取方本来也是分头取的：模式找
 * UGeLocomotionComponent，层实现类和脚部 IK 开关找本组件。
 */
UCLASS(ClassGroup=(Animation), meta=(BlueprintSpawnableComponent))
class GENIUXYANIMATION_API UGeAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGeAnimationComponent();

	/** 便捷查找：没有就返回 nullptr */
	static UGeAnimationComponent* Find(const AActor* InActor);

	UFUNCTION(BlueprintCallable)
	void SetAnimationSet(UCharacterAnimationSet* InAnimationSet);

	UFUNCTION(BlueprintCallable)
	bool PlayAction(FGameplayTag ActionTag);

	/**********************************************************************/
	/*                          Animation Setup                           */
	/**********************************************************************/
public:
	/**
	 * 每角色的动画配置（层实现类 + 脚部 IK 开关），运行时由本组件持有。
	 *
	 * 配置入口只有一个：UPA_CharacterDefinition::AnimationSetup（主数据资产，和蒙太奇资产
	 * 同一个入口），由角色在拿到主数据资产后调 SetAnimationSetup 推过来。
	 * 故意不做成 EditDefaultsOnly、也不在 BP 上挂：否则又会变成
	 * 「BP 和资产两处都能填、最后不知道该看哪个」。
	 */
	UFUNCTION(BlueprintPure, Category = "Geniuxy|Animation")
	UGeAnimationSetup* GetAnimationSetup() const { return AnimationSetup; }

	/** 设置动画配置：同步脚部 IK 开关，并广播 OnAnimationSetupChanged 让 AnimInstance 重新绑层 */
	UFUNCTION(BlueprintCallable, Category = "Geniuxy|Animation")
	void SetAnimationSetup(UGeAnimationSetup* InSetup);

	/** 配置变化时广播（角色据此让 AnimInstance 重新 LinkAnimClassLayers） */
	FOnGeAnimationSetupChanged OnAnimationSetupChanged;

	/**********************************************************************/
	/*                              Foot IK                               */
	/**********************************************************************/
public:
	/**
	 * 是否允许脚部 IK。
	 *
	 * 这是**运行时值**，手工配置只有一个入口：UGeAnimationSetup::bCanFootIK
	 * （由 PA_CharacterDefinition::AnimationSetup 指定，SetAnimationSetup 时同步过来）。
	 * 故意不做成 EditDefaultsOnly，就是为了避免「两个地方都能填、最后不知道该看哪个」。
	 *
	 * 为什么不放在 AnimInstance 的 Class Defaults 里：
	 * 主实例和 layer 实例是两个独立 UAnimInstance，各有一份类默认值；
	 * 在 layer ABP 上改永远影响不到主 ABP，而主 ABP 只有一份，也没法按武将区分。
	 */
	UFUNCTION(BlueprintPure, Category = "Geniuxy|FootIK")
	bool IsFootIKEnabled() const { return bFootIKEnabled; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|FootIK")
	bool IsClimbIKEnabled() const { return bClimbIKEnabled; }

	/** 运行时临时开关（一般不用手动调，配 UGeAnimationSetup 即可） */
	UFUNCTION(BlueprintCallable, Category = "Geniuxy|FootIK")
	void SetFootIKEnabled(bool bInEnabled) { bFootIKEnabled = bInEnabled; }

protected:
	/**
	 * 脚部 IK 的运行时值。默认 true。
	 * 手工配置的唯一入口是 UGeAnimationSetup::bCanFootIK，这里刻意不加 EditDefaultsOnly。
	 */
	bool bFootIKEnabled = true;
	bool bClimbIKEnabled = true;

private:
	bool PlayLoadedMontage(const FCharacterMontageEntry& Entry);

private:
	UPROPERTY()
	TObjectPtr<UCharacterAnimationSet> AnimationSet;

	/**
	 * 运行时由 UPA_CharacterDefinition 推过来的动画配置。
	 * 标 Transient：不参与序列化，免得这份数据又从 BP 里被「救」回来。
	 */
	UPROPERTY(Transient, VisibleAnywhere, Category = "Geniuxy|Animation")
	TObjectPtr<UGeAnimationSetup> AnimationSetup;
};
