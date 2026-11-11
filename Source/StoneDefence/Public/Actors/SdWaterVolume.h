// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/PhysicsVolume.h"
#include "SdWaterVolume.generated.h"

class UGameplayEffect;

/**
 * 水池体积：进出水时给目标挂 / 摘 Ge.Stats.Swim。
 *
 * 为什么必须是 APhysicsVolume：引擎的游泳判定只认 APhysicsVolume::bWaterVolume
 * （UCharacterMovementComponent::PhysicsVolumeChanged）。普通 TriggerBox 不够 ——
 * 只挂 tag 而没有水体体积，PhysSwimming 每帧会把人踢回 MOVE_Falling。
 *
 * 摆法：
 * 1. 盖住水池。形状随意（brush 体积，圆柱 / 挤出顶点都行），**顶面就是引擎认定的水面**，
 *    别低于视觉水面，否则人要沉到水下才判定进水；
 * 2. SM_WaterMesh 的碰撞设成 NoCollision，池底要给碰撞（不然会掉出体积底面）；
 * 3. Priority 默认 10，比默认物理体积高，一般不用动。
 *
 * 挂上 tag 之后 UGeLocomotionComponent 会收到 tag 事件，把移动模式切到 Swim。
 * 浮力等手感参数在角色侧（ASdCharacterBase::SwimBuoyancy），不在这里做。
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
	 * 默认是原生 UGE_Stats_Swim（无限时长，只挂 Ge.Stats.Swim）；
	 * 想加别的东西就在蓝图里继承它再换到这里。
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
