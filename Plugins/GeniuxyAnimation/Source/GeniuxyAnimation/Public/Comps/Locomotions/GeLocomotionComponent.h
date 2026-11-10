// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GeniuxyAnimationType.h"
#include "Components/ActorComponent.h"
#include "GeLocomotionComponent.generated.h"

class UCharacterMovementComponent;
class UAbilitySystemComponent;
class UGeAnimationSetup;

/** 动画配置变化（非同动态委托，C++ 侧用） */
DECLARE_MULTICAST_DELEGATE(FOnGeAnimationSetupChanged);

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
 * - 它同时也是脚部 IK 开关的数据源（角色配置推过来，AnimInstance 读它）。
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
	/*                           Animation Setup                          */
	/**********************************************************************/
	/**
	 * 每角色的动画配置（层实现类 + 脚部 IK 开关）。
	 *
	 * 放在组件上而不是角色上，理由：
	 * - 它和「移动模式」属于同一类数据，都是 AnimInstance 要读的东西；
	 * - 主实例和 layer 实例本来就都从本组件取数据，不用再多绕一层角色；
	 * - 换配置时的重新绑层也能由组件广播事件驱动，角色只当转发者。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Geniuxy|Animation")
	UGeAnimationSetup* AnimationSetup;

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
	/**
	 * 是否允许脚部 IK。
	 *
	 * 这是**运行时值**，手工配置只有一个入口：UGeAnimationSetup::bCanFootIK
	 * （SetAnimationSetup / BeginPlay 时同步过来）。故意不做成 EditDefaultsOnly，
	 * 就是为了避免「两个地方都能填、最后不知道该看哪个」。
	 *
	 * 为什么不放在 AnimInstance 的 Class Defaults 里：
	 * 主实例和 layer 实例是两个独立 UAnimInstance，各有一份类默认值；
	 * 在 layer ABP 上改永远影响不到主 ABP，而主 ABP 只有一份，也没法按武将区分。
	 */
	UFUNCTION(BlueprintPure, Category = "Geniuxy|FootIK")
	bool IsFootIKEnabled() const { return bFootIKEnabled; }

	/** 运行时临时开关（一般不用手动调，配 UGeAnimationSetup 即可） */
	UFUNCTION(BlueprintCallable, Category = "Geniuxy|FootIK")
	void SetFootIKEnabled(bool bInEnabled) { bFootIKEnabled = bInEnabled; }

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
	void HandleFlyFastTagChanged(const FGameplayTag Tag, int32 NewCount);
	void HandleSwimTagChanged(const FGameplayTag Tag, int32 NewCount);
	void HandleSwimFastTagChanged(const FGameplayTag Tag, int32 NewCount);
	void HandleClimbTagChanged(const FGameplayTag Tag, int32 NewCount);

	/** 只有服务器 / 本地控制端才去改 MovementMode，模拟端只跟随复制 */
	bool ShouldDriveMovement() const;

protected:
	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY()
	UAbilitySystemComponent* OwnerASC;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;

	/**
	 * 脚部 IK 的运行时值。默认 true。
	 * 手工配置的唯一入口是 UGeAnimationSetup::bCanFootIK，这里刻意不加 EditDefaultsOnly。
	 */
	bool bFootIKEnabled = true;

	EGeLocomotionMode Mode = EGeLocomotionMode::Ground;
	EGeLocomotionMode PreviousMode = EGeLocomotionMode::Ground;

	/** 模式栈：被飞行/攀爬/游泳打断前的模式（只服务于手动 PushMode/PopMode） */
	TArray<EGeLocomotionMode> ModeHistory;

	/** 来自 GAS tag 的原始状态，供 RefreshModeFromTags 做优先级推导 */
	bool bFlyTagActive = false;
	bool bFlyFastTagActive = false;
	bool bSwimTagActive = false;
	bool bSwimFastTagActive = false;
	bool bClimbTagActive = false;

	FDelegateHandle FlyTagHandle;
	FDelegateHandle FlyFastTagHandle;
	FDelegateHandle SwimTagHandle;
	FDelegateHandle SwimFastTagHandle;
	FDelegateHandle ClimbTagHandle;

public:
	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion")
	bool IsFlyingFast() const { return bFlyFastTagActive; }
	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion")
	bool IsSwimmingFast() const { return bSwimFastTagActive; }
};
