// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GeGameplayEffectSwim.generated.h"

/**
 * 「游泳中」状态 GE：给目标挂上 Ge.Stats.Swim，摘掉时自动移除。
 *
 * 为什么做成原生 C++ 类而不是 BP 资产：
 * - 它只表达「我带着游泳状态」这一件事，没有任何数值 / 曲线 / 资产依赖；
 * - 水池体积（ASdWaterVolume）拿到就能直接用，不用先要求有人去 Content 里建一个 GE 资产；
 * - 真要改（比如顺便加个属性减速），在蓝图里继承本类再覆写即可，体积上的
 *   SwimGameplayEffect 换一下就行。
 *
 * 注意 UE5.3+ 的模块化改动：
 * 「授予目标标签」已经由 UTargetTagsGameplayEffectComponent 负责，
 * 老的 InheritableOwnedTagsContainer 被标了弃用。运行时真正读的是
 * UGameplayEffect::GetGrantedTags() -> CachedGrantedTags，所以这里的做法是
 * 建组件 -> 入 GEComponents 队列 -> SetAndApplyTargetTagChanges 把缓存写好。
 *
 * 用法（服务器权威）：
 *   ASC->ApplyGameplayEffectToSelf(GetDefault<UGeGameplayEffectSwim>(), 1.f, Context);
 * 无限时长，靠 RemoveActiveGameplayEffect(FActiveGameplayEffectHandle) 移除。
 * 挂上后 UGeLocomotionComponent 会收到 Ge.Stats.Swim 的 tag 事件，
 * 自动把移动模式切到 Swim / MOVE_Swimming。
 */
UCLASS()
class GENIUXYGAS_API UGeGameplayEffectSwim : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGeGameplayEffectSwim();
};
