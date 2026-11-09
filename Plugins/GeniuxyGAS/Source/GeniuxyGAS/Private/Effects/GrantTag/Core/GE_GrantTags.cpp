// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/GrantTag/Core/GE_GrantTags.h"

#include "AbilitySystemLog.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UGE_GrantTags::UGE_GrantTags()
{
	// 这类 GE 都是「谁挂上谁摘掉」，不靠时间自然结束。
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	// 其它默认值（堆叠、周期、修饰器）全部保持引擎默认：本类只负责挂标签。

	TargetTagsComponent = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTags"));

	if (!TargetTagsComponent)
	{
		// 理论上不会发生；真发生了就说明子类的标签全都挂不上，宁可喊一声也别静默。
		UE_LOG(LogGameplayEffects, Error,
			TEXT("[GE_GrantTags] %s 创建 TargetTags 组件失败，授予的标签不会生效"), *GetName());
		return;
	}

	// 必须在这里入列：UGameplayEffect::PostInitProperties 只会在 DefaultSubobject 里
	// 捞漏网的组件并 ensure 报错（原生 GE 没有 PostLoad，这是它唯一的兜底时机）。
	GEComponents.Add(TargetTagsComponent);
}

void UGE_GrantTags::AddGrantedTag(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGameplayEffects, Warning,
			TEXT("[GE_GrantTags] %s 收到一个无效标签，已忽略"), *GetName());
		return;
	}

	TagChanges.AddTag(Tag);
	FlushGrantedTags();
}

void UGE_GrantTags::AddGrantedTags(const FGameplayTagContainer& Tags)
{
	bool bChanged = false;

	for (const FGameplayTag& Tag : Tags)
	{
		if (Tag.IsValid())
		{
			TagChanges.AddTag(Tag);
			bChanged = true;
		}
	}

	if (bChanged)
	{
		FlushGrantedTags();
	}
}

void UGE_GrantTags::FlushGrantedTags()
{
	if (!TargetTagsComponent)
	{
		return;
	}

	// SetAndApplyTargetTagChanges 是整份替换：把当前累积的全部标签交给组件，
	// 组件内部会重算 CombinedTags 并写回 GE 的 CachedGrantedTags。
	TargetTagsComponent->SetAndApplyTargetTagChanges(TagChanges);
}
