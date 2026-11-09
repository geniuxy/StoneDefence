// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/GeGameplayEffectSwim.h"

#include "AbilitySystemLog.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Tags/StatsTags.h"

UGeGameplayEffectSwim::UGeGameplayEffectSwim()
{
	// 无限时长：进出水由 ASdWaterVolume 手动上 / 下，不靠时间自然结束。
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	// 其它默认值（堆叠、周期、修饰器）全部保持引擎默认：本 GE 只负责挂 tag。

	// 5.3 起「授予目标标签」必须走 GEComponent。
	// 组件必须在构造函数里就进 GEComponents 队列 —— UGameplayEffect::PostInitProperties 只会在
	// DefaultSubobject 里捞漏网的组件并 ensure 报错（原生 GE 没有 PostLoad，它只能这么兜底）。
	UTargetTagsGameplayEffectComponent* TargetTags =
		CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTags"));

	if (!TargetTags)
	{
		// 理论上不会发生；真发生了就说明这个 GE 挂不上游泳 tag，宁可喊一声也别静默。
		UE_LOG(LogGameplayEffects, Error, TEXT("[UGeGameplayEffectSwim] 创建 TargetTags 组件失败，Ge.Stats.Swim 不会被授予"));
		return;
	}

	GEComponents.Add(TargetTags);

	FInheritedTagContainer TagChanges;
	TagChanges.AddTag(StatsTags::Ge_Stats_Swim);

	// 这一步把标签写进 GE 的 CachedGrantedTags —— 运行时 ApplyGameplayEffectSpec 读的就是这份缓存。
	// 只 AddTag 不调用它的话，缓存是空的，tag 挂不上（这是原生 GE 最容易踩的坑）。
	TargetTags->SetAndApplyTargetTagChanges(TagChanges);
}
