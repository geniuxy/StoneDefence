// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GeCombatStateProvider.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UGeCombatStateProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 「战斗状态」提供者 —— 动画侧与 GAS 侧之间的依赖倒置接口。
 *
 * 为什么需要它（而不是让动画插件直接依赖 GeniuxyGAS）：
 * - 战斗状态的权威来源是 GAS 的 Ge.Stats.InCombat tag，监听点应该在 UGeAbilitySystemComponent；
 * - 但 AnimInstance 每帧都要读这个状态，如果它直接 `#include` GAS 插件的类，
 *   GeniuxyAnimation 就反向依赖了 GeniuxyGAS（动画插件再也无法脱离 GAS 单独使用）；
 * - 把接口放在两边都依赖的 GeniuxyCommon 里，方向就正过来了：
 *       GeniuxyGAS       —— 实现（UGeAbilitySystemComponent : public IGeCombatStateProvider）
 *       GeniuxyAnimation —— 消费（Cast<IGeCombatStateProvider>(ASC) 后读缓存值）
 *
 * 实现方的约定（很重要）：
 *   IsInCombat() 必须返回**已经缓存好的 bool**，绝对不能在里面做 tag 查找 / 遍历容器 ——
 *   调用方是「每个 AnimInstance 每帧一次」，这里多一次 TMap 查找就等于回到了轮询。
 *   状态变化由实现方自己监听（UGeAbilitySystemComponent 的 tag 回调），消费方只读结果。
 *
 * 以后要给动画侧加别的状态（游泳 / 攀爬 / 持武……），照这个模式加：
 *   - 状态少、总是一起用 → 直接在本接口上加纯虚函数，两边同步改；
 *   - 状态各有各的归属 → 再加一个同级的 IGeXxxProvider，别把接口堆成上帝对象。
 */
class GENIUXYCOMMON_API IGeCombatStateProvider
{
	GENERATED_BODY()

public:
	/** 当前是否在战斗中（实现方返回缓存值，不做任何查找） */
	virtual bool IsInCombat() const = 0;
};
