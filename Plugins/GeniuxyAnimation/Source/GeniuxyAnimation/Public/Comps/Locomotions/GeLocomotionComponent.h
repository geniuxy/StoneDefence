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
	
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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
	/*                           Swim | Buoyancy                          */
	/**********************************************************************/
	/*
	 * 游泳时按「浸没深度」自动切换浮力，这才有「松开下潜键停在水下、按空格才浮上去」：
	 *   - 完全没入水面下（ImmersionDepth >= SwimUnderwaterDepth）→ 中性浮力（悬停，不会自己飘上去）；
	 *   - 水面附近、且没在按「下潜」→ 恢复角色原本的浮力（浮在水面）。
	 *
	 * 怎么知道在按「下潜」：读 GAS 上 GA_Swim_Dive 挂的 Ge.Stats.Swim.Dive tag ——
	 * 不直接依赖那个能力类，所以本组件不需要依赖 GeniuxyGAS。
	 *
	 * 「角色原本的浮力」不在这里再配一份：第一次需要改成中性时把 CMC 当时的值缓存下来，
	 * 回到水面 / 离开游泳时还原 —— ASdCharacterBase::SwimBuoyancy 仍是唯一配置入口。
	 */
public:
	/** 当前是否已经潜到水下（完全浸没）。动画/UI 用它比按速度猜准 */
	UFUNCTION(BlueprintPure, Category = "Geniuxy|Swim")
	bool IsUnderwater() const { return bSwimUnderwater; }

protected:
	/** 水下（完全浸没）时用的浮力：1.0 = 中性，停在水下不上浮 */
	UPROPERTY(EditDefaultsOnly, Category = "Geniuxy|Swim", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float SwimUnderwaterBuoyancy = 1.0f;

	/** 浸没比例超过它就算「在水下」。1.0 = 完全浸没，0.95 ≈ 头刚没入 */
	UPROPERTY(EditDefaultsOnly, Category = "Geniuxy|Swim", meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float SwimUnderwaterDepth = 0.95f;

	/**
	 * 上浮最多浮到这个浸没比例，再往上就停住（水面稳定带）。
	 *
	 * 为什么需要：体积的出水判定看「胶囊中心还在不在 brush 里」，而 Depth = 0.5 正好对应
	 * 胶囊中心贴在 brush 顶面。浮力平衡点（1/Buoyancy ≈ 0.51）几乎压在这个边界上，
	 * 于是人一到水面就反复越界：出水摘 tag → 引擎切 MOVE_Falling → 落回来又入水，
	 * 玩家看到的就是「浮出水面那一瞬间掉出游泳状态」。
	 *
	 * 这里卡一个余量：中心升到「水面下方 (Depth-0.5)*胶囊高」就停止上升，并轻轻压回带内。
	 * 0.55 ≈ 中心在水面下方 9cm。想让角色浮得更高就往 0.52 调（余量变小、更容易抖）。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Geniuxy|Swim", meta = (ClampMin = "0.5", ClampMax = "0.9"))
	float SwimSurfaceKeepDepth = 0.55f;

	/** 被压回稳定带时的下沉速度（cm/s）。太小会卡在水面边界上磨，太大能看出被拽 */
	UPROPERTY(EditDefaultsOnly, Category = "Geniuxy|Swim", meta = (ClampMin = "0.0"))
	float SwimSurfacePushDownSpeed = 60.f;

	/**
	 * 松手后水平方向的减速度（cm/s²）。0 = 不管（维持引擎的"缓慢滑行"）。
	 *
	 * 为什么不用 CMC 自带的 BrakingDecelerationSwimming / FluidFriction：
	 * 那两个都作用在**完整速度矢量**上，一调大就连竖直方向一起刹掉，
	 * 而完全浸没时浮力只有 ~0.95g（≈930cm/s²），刹得比它狠人就浮不上来了。
	 * 这里只处理 XY，竖直方向完全交给浮力 / 下潜 / 空格。
	 *
	 * 4000 ≈ 300cm/s 的水平速度 0.075 秒停住（"松手马上停"）；
	 * 想更硬给 8000，想要一点滑行感给 1500。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Geniuxy|Swim", meta = (ClampMin = "0.0"))
	float SwimHorizontalBrakeDecel = 4000.f;

	/**
	 * 「多久没有移动输入才算真的松手」的迟滞（秒）。
	 * 判定用的是 CMC 的 Acceleration，而输入相位 / 网络抖动可能让某一帧出现空档，
	 * 迟滞能避免那种单帧空档把速度误刹掉。默认 0.03s ≈ 2 帧。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Geniuxy|Swim", meta = (ClampMin = "0.0", ClampMax = "0.2"))
	float SwimBrakeNoInputDelay = 0.03f;

	/** 打开后每次水平刹车都会打一行（PIE 屏幕 + 日志），用来确认客户端上到底有没有刹 */
	UPROPERTY(EditAnywhere, Category = "Geniuxy|Swim|Debug")
	bool bDebugSwimBrake = false;

private:
	/** 每帧按浸没深度 + 下潜 tag 决定浮力；只有 Swim 模式才开 tick */
	void UpdateSwimBuoyancy();

	/** 松手后只刹水平速度（竖直交给浮力），避免游泳时"松了还一直在滑" */
	void UpdateSwimHorizontalBrake(float DeltaTime);

	/** 交回角色原本的浮力（缓存值）；没缓存过就什么都不做 */
	void RestoreSurfaceBuoyancy();

	/** 角色原本的浮力（第一次改成中性前从 CMC 上抓下来的值） */
	float CachedSurfaceBuoyancy = 0.f;
	bool bSwimBuoyancyCached = false;
	bool bSwimUnderwater = false;

	/** 已经连续多久没有移动输入（用于水平刹车的迟滞） */
	float SwimNoInputTime = 0.f;

	/**********************************************************************/
	/*                           Animation Setup                          */
	/**********************************************************************/
public:
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
