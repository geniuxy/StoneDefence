// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/PhysicsVolume.h"
#include "SdWaterVolume.generated.h"

class UGameplayEffect;

/**
 * 水池体积：勾好「Water Volume」+ 进出水自动挂 / 摘 Ge.Stats.Swim。
 *
 * 为什么要做成 APhysicsVolume 而不是 TriggerBox：
 * - 引擎的游泳是靠体积判定的，不是靠 tag。「是不是水」的唯一判据是
 *   APhysicsVolume::bWaterVolume（UCharacterMovementComponent::PhysicsVolumeChanged 里读它）；
 * - 只挂 tag 不加体积，PhysSwimming 每帧会检查 GetPhysicsVolume()->bWaterVolume，
 *   非水就 SetMovementMode(MOVE_Falling)，人根本游不住；
 * - 浮力 / 浸没深度（ImmersionDepth）也是拿体积 brush 竖直打线算的，所以体积顶面
 *   就是「水面高度」，必须和 SM_WaterMesh 的视觉水面齐平（建议略高一点点，见下）。
 *
 * 它同时负责状态层：让物理层和 UGeLocomotionComponent 的 Mode 保持一致。
 *   - 物理层：引擎自己切 MOVE_Swimming；
 *   - 状态层：本体积在服务器上给 ASC 上 / 下 Ge.Stats.Swim，
 *     UGeLocomotionComponent 收到 tag 事件后 SetMode(Swim) 驱动动画 / 速度 / 层切换。
 * 只有体积没有 tag 的话，人会游但播走路动画（Mode 还停在 Ground）；只有 tag 没有体积的话，
 * 每帧被踢回 MOVE_Falling。两个都要。
 *
 * 关卡里怎么摆：
 * 1. 拖一个本类（或 PhysicsVolume 勾上 Water Volume）盖住水池；
 * 2. 顶面贴水面（判定点 = 胶囊中心，所以顶面比视觉水面高 10~20cm 更稳，
 *    否则在表面上下会「出体积→Falling→又进体积」地抖）；
 * 3. 池底给碰撞（别让人从体积底面掉出去），SM_WaterMesh 的碰撞设成 NoCollision；
 * 4. Priority 已默认 10，比默认物理体积高，一般不用动；
 *    手感不对调 FluidFriction（PhysSwimming 里用的是 0.5 * FluidFriction * Depth）。
 *
 * 角色侧还要配：CharacterMovement 的 MaxSwimSpeed（默认 300）、Buoyancy
 * （=1 时只有完全浸没才抵消重力，想稳定浮在水面用 1.2~1.5，平衡点 Depth = 1/Buoyancy）。
 *
 * 已知边界：
 * - 出生点直接摆在水里时，角色的 ASC 可能还没初始化完（没 Possess / 没 InitAbilityActorInfo），
 *   这一帧挂不上 tag；把 PlayerStart 放在水外即可避开；
 * - 两个水体体积重叠时，先离开的那个会摘 tag，重叠区域请避免，或者自己按需扩展。
 */
UCLASS(Blueprintable)
class STONEDEFENCE_API ASdWaterVolume : public APhysicsVolume
{
	GENERATED_BODY()

public:
	ASdWaterVolume();

	//~ Begin APhysicsVolume Interface
	virtual void ActorEnteredVolume(AActor* Other) override;
	virtual void ActorLeavingVolume(AActor* Other) override;
	//~ End APhysicsVolume Interface

	/** 这个 actor 身上有没有本体积挂上去的游泳 GE（调试 / 其它系统查询用） */
	UFUNCTION(BlueprintPure, Category = "Swim")
	bool IsSwimTagApplied(AActor* Other) const;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * 授予游泳状态的 GE。
	 * 默认是原生 UGeGameplayEffectSwim（无限时长，只挂 Ge.Stats.Swim）；
	 * 想加「水里减速」这类额外效果，在蓝图里继承 UGeGameplayEffectSwim 再换到这里即可。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Swim")
	TSubclassOf<UGameplayEffect> SwimGameplayEffect;

	/** 进出水要不要打调试信息（只影响 PIE 屏幕 + 日志，打包版 Debug::Print 本来就静默） */
	UPROPERTY(EditAnywhere, Category = "Swim|Debug")
	bool bPrintDebug = true;

private:
	/** 服务器侧：给 actor 的 ASC 上 Ge.Stats.Swim */
	void ApplySwimTag(AActor* Other);

	/** 服务器侧：摘掉本体积挂上去的那份 GE */
	void RemoveSwimTag(AActor* Other);

	/** 每个 actor 挂上去的那份 GE 句柄 —— 出水时要按句柄摘，不能按 tag 瞎移除别人的效果 */
	TMap<TWeakObjectPtr<AActor>, FActiveGameplayEffectHandle> SwimEffectHandles;
};
