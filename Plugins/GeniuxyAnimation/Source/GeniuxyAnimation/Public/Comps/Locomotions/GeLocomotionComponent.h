// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GeniuxyAnimationType.h"
#include "Components/ActorComponent.h"
#include "GeLocomotionComponent.generated.h"

class UCharacterMovementComponent;
class UAbilitySystemComponent;
class UGeLocomotionAnimSet;

/** 模式变化（蓝图侧） */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnGeLocomotionModeChangedBP, EGeLocomotionMode, OldMode, EGeLocomotionMode, NewMode);

/** 模式变化（C++ 侧） */
DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnGeLocomotionModeChanged, EGeLocomotionMode /*OldMode*/, EGeLocomotionMode /*NewMode*/);

/**
 * 统一的移动模式管理器：Ground / Fly / Swim / Climb。
 *
 * 职责边界（很重要）：
 * - 它只做「状态」和「MovementMode 切换」，不碰动画图；
 * - 它是移动模式的唯一数据源。AnimInstance 每帧来这里读一次，不要在 AnimInstance 里
 *   再注册一遍 GAS tag 事件（旧的 AnimInstance 和飞行组件各注册过一份，是重复监听）；
 * - 它持有 UGeLocomotionAnimSet，主实例和所有 layer 实例都从这里取动画资产，
 *   这样「跨 AnimInstance 传数据」的问题就退化成了「读同一个组件的指针」。
 *
 * 模式来源优先级：GAS tag（Fly / Swim / Climb）驱动进出栈；也可以直接调 SetMode/PushMode。
 */
UCLASS(ClassGroup=(GeniuxyAnimation), meta=(BlueprintSpawnableComponent))
class GENIUXYANIMATION_API UGeLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGeLocomotionComponent();

	/** 便捷查找：没有就返回 nullptr */
	static UGeLocomotionComponent* Find(const AActor* InActor);

	/**********************************************************************/
	/*                               Mode                                 */
	/**********************************************************************/
	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion")
	EGeLocomotionMode GetMode() const { return Mode; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion")
	EGeLocomotionMode GetPreviousMode() const { return PreviousMode; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion")
	bool IsInMode(EGeLocomotionMode InMode) const { return Mode == InMode; }

	/** 直接切模式，不记录历史 */
	UFUNCTION(BlueprintCallable, Category = "Geniuxy|Locomotion")
	void SetMode(EGeLocomotionMode InNewMode);

	/** 进栈：记录当前模式后切换。飞行/攀爬/游泳这类「会结束并回到之前状态」的用它 */
	UFUNCTION(BlueprintCallable, Category = "Geniuxy|Locomotion")
	void PushMode(EGeLocomotionMode InNewMode);

	/** 出栈：回到最近一次 Push 之前的模式；栈空则回 Ground */
	UFUNCTION(BlueprintCallable, Category = "Geniuxy|Locomotion")
	void PopMode();

	/** 在拥有者角色的同步状态里取模式；没有组件时回退为 Ground */
	static EGeLocomotionMode GetModeFromActor(const AActor* InActor);

	/**********************************************************************/
	/*                              Combat                                */
	/**********************************************************************/
	/** 战斗状态。和移动模式正交，可以边飞边打，所以单独存 */
	UFUNCTION(BlueprintPure, Category = "Geniuxy|Combat")
	bool IsInCombat() const { return bInCombat; }

	/**********************************************************************/
	/*                            Animation Set                           */
	/**********************************************************************/
	/** 设置移动动画集合（会顺带调用 PreloadAll，游戏线程） */
	UFUNCTION(BlueprintCallable, Category = "Geniuxy|Animation")
	void SetAnimationSet(UGeLocomotionAnimSet* InAnimSet);

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Animation")
	UGeLocomotionAnimSet* GetAnimationSet() const { return AnimationSet; }

	/**********************************************************************/
	/*                             Delegates                              */
	/**********************************************************************/
	FOnGeLocomotionModeChanged OnModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Geniuxy|Locomotion")
	FOnGeLocomotionModeChangedBP OnModeChangedBP;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * 把 EGeLocomotionMode 翻译成 UCharacterMovementComponent 的移动模式。
	 *
	 * 默认实现：Ground->Walking，Fly->Flying，Swim->Swimming，Climb->Flying。
	 * Climb 之所以默认用 Flying，是因为 MOVE_Custom 必须由自研的
	 * UCharacterMovementComponent 重写 PhysCustom 才有行为，直接用会「人不动」。
	 * 等你们写了自定义移动组件，在蓝图或 C++ 里重载这个事件改成 MOVE_Custom 即可。
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Geniuxy|Locomotion")
	void ApplyMovementMode(EGeLocomotionMode InMode);
	virtual void ApplyMovementMode_Implementation(EGeLocomotionMode InMode);

private:
	void ChangeModeInternal(EGeLocomotionMode InNewMode, bool bPushHistory);
	void BroadcastModeChanged(EGeLocomotionMode InOldMode);

	/**
	 * 由「当前有哪些 tag 在生效」推导模式，优先级 Climb > Fly > Swim > Ground。
	 *
	 * 只在 tag 状态真的变化时调用，所以手动 SetMode 的结果不会被每帧覆盖掉。
	 * 用优先级而不是栈，是为了避免「飞行中触发攀爬，攀爬结束后卡在飞行」这类栈错位。
	 */
	void RefreshModeFromTags();

	void BindAbilitySystemTags();
	void UnbindAbilitySystemTags();

	void HandleFlyTagChanged(const FGameplayTag Tag, int32 NewCount);
	void HandleSwimTagChanged(const FGameplayTag Tag, int32 NewCount);
	void HandleClimbTagChanged(const FGameplayTag Tag, int32 NewCount);
	void HandleCombatTagChanged(const FGameplayTag Tag, int32 NewCount);

	/** 只有服务器 / 本地控制端才去改 MovementMode，模拟端只跟随复制 */
	bool ShouldDriveMovement() const;

protected:
	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY()
	UAbilitySystemComponent* OwnerASC;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;

	UPROPERTY()
	UGeLocomotionAnimSet* AnimationSet;

	EGeLocomotionMode Mode = EGeLocomotionMode::Ground;
	EGeLocomotionMode PreviousMode = EGeLocomotionMode::Ground;

	/** 模式栈：被飞行/攀爬/游泳打断前的模式（只服务于手动 PushMode/PopMode） */
	TArray<EGeLocomotionMode> ModeHistory;

	bool bInCombat = false;

	/** 来自 GAS tag 的原始状态，供 RefreshModeFromTags 做优先级推导 */
	bool bFlyTagActive = false;
	bool bSwimTagActive = false;
	bool bClimbTagActive = false;

	FDelegateHandle FlyTagHandle;
	FDelegateHandle SwimTagHandle;
	FDelegateHandle ClimbTagHandle;
	FDelegateHandle CombatTagHandle;
};
