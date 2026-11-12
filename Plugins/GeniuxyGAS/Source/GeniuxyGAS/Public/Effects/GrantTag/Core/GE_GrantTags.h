// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_GrantTags.generated.h"

class UTargetTagsGameplayEffectComponent;

/**
 * 公共基类：「只负责给目标挂状态标签」的 GE。
 *
 * 为什么需要这个基类：
 * UE5.3 起给目标挂标签的唯正确路径是 UTargetTagsGameplayEffectComponent
 * （老的 InheritableOwnedTagsContainer 已弃用），而运行时真正读的是
 * UGameplayEffect::GetGrantedTags() -> CachedGrantedTags 这份**缓存**。
 * 这份缓存只有两处会填：
 *   - UTargetTagsGameplayEffectComponent::SetAndApplyTargetTagChanges()
 *   - UGameplayEffect::OnGameplayEffectChanged()（资产加载 / 编辑器里改属性时重新聚合）
 * 少调前者就会得到「GE 上看着配了标签，运行时一个都没挂上」的经典坑；
 * 另外原生 GE 没有 PostLoad，组件还必须在本类构造函数里就进 GEComponents 队列，
 * 否则 UGameplayEffect::PostInitProperties 会 ensure 报错。
 * 这套固定动作没有复用价值、每次都手写又容易漏，所以收在基类里。
 *
 * 用法（C++ 子类，构造函数里声明要挂哪些标签）：
 *   UGE_Stats_Swimming::UGE_Stats_Swimming()
 *   {
 *       AddGrantedTag(StatsTags::Ge_Stats_Swimming);
 *   }
 *
 * 用法（蓝图子类，直接建 GE 资产）：
 *   Details 面板 -> Components -> Target Tags (Granted to Actor) -> Add Tags 里填标签。
 *   基类已经建好这个组件了，不要再手动加第二个（同类型组件重复，引擎会报 invalid）。
 *
 * 时长固定 Infinite：这类状态标签都是「谁挂上谁摘掉」，
 * 由挂的人拿着 Apply 返回的 FActiveGameplayEffectHandle 调 RemoveActiveGameplayEffect。
 */
UCLASS(Abstract)
class GENIUXYGAS_API UGE_GrantTags : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_GrantTags();

protected:
	/** 追加一个要授予目标的标签；在子类构造函数里调用，可多次调用 */
	void AddGrantedTag(const FGameplayTag& Tag);

	/** 追加一批要授予目标的标签；在子类构造函数里调用 */
	void AddGrantedTags(const FGameplayTagContainer& Tags);

private:
	/** 把累积的标签整体写进组件，并刷新 GE 的 CachedGrantedTags */
	void FlushGrantedTags();

	/** 本 GE 用来授予标签的组件（由构造函数创建，同时也在 GEComponents 里） */
	UPROPERTY()
	TObjectPtr<UTargetTagsGameplayEffectComponent> TargetTagsComponent;

	/** 本类累积的标签变更，最后整体交给组件（SetAndApplyTargetTagChanges 是整份替换） */
	FInheritedTagContainer TagChanges;
};
