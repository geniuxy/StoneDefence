// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PA_AbilitySystemGenerics.h"

#if WITH_EDITOR
#include "Abilities/GameplayAbility.h"
#include "UObject/UnrealType.h"

namespace
{
	/**
	 * 从 CDO 上按属性名读一个 FGameplayTagContainer，把里面的 Tag 名塞进 OutTagStrings。
	 *
	 * 为什么要走反射而不是直接点成员：
	 * UGameplayAbility::ActivationRequiredTags / ActivationBlockedTags 是 **protected** 的
	 * （只有 AbilityTags 是 public），类外读不到。校验代码不该为此去动引擎类，
	 * 也不该强制所有 GA 都继承某个带访问器的基类 —— 反射对任意 UGameplayAbility 子类（含蓝图子类）都成立。
	 *
	 * @return 属性存在且类型正确返回 true
	 */
	bool CollectTagContainerProperty(
		const UObject* CDO, const TCHAR* PropertyName, TArray<FString>& OutTagStrings)
	{
		const FStructProperty* Property = FindFProperty<FStructProperty>(CDO->GetClass(), FName(PropertyName));
		if (!Property || Property->Struct != FGameplayTagContainer::StaticStruct())
		{
			return false;
		}

		// Property 是 const 的，但取值的指针参数本身是 void*（不会写入），这里把 const 剥掉用
		const FGameplayTagContainer* Container =
			Property->ContainerPtrToValuePtr<FGameplayTagContainer>(const_cast<UObject*>(CDO));
		if (!Container)
		{
			return false;
		}

		for (const FGameplayTag& Tag : *Container)
		{
			OutTagStrings.Add(Tag.ToString());
		}

		return true;
	}

	/**
	 * 把一条能力条目的「门控」拼成可比较的字符串。
	 *
	 * 取的是 GA 类默认对象上的三组 Tag：
	 * - AbilityTags：这条能力属于哪个分组（会被别人的 BlockAbilitiesWithTag / CancelAbilitiesWithTag 命中）
	 * - ActivationRequiredTags / ActivationBlockedTags：自己要求的 ASC Tag 条件
	 * 排序后拼接，避免「同样的 Tag 集合、顺序不同」被当成两个不同的门控。
	 *
	 * 刻意不包含 ActivationOwnedTags / SourceXxxTags / TargetXxxTags：
	 * 它们不决定「按这个键时会不会被激活」，混进来只会让两份实际冲突的配置看起来不一样。
	 *
	 * @return false 表示读不到（没配类，或引擎属性改名了），调用方应当跳过这条，别当成「无门控」
	 */
	bool MakeAbilityGateSignature(const FGeAbilitySpecEntry& Entry, FString& OutSignature)
	{
		if (!Entry.AbilityClass)
		{
			return false;
		}

		const UGameplayAbility* AbilityCDO = Entry.AbilityClass->GetDefaultObject<UGameplayAbility>();
		if (!AbilityCDO)
		{
			return false;
		}

		TArray<FString> TagStrings;

		// 用 | 而不是 ||：三组都要试一遍，不能因为第一组读到了就把后两组短路掉
		const bool bFoundAny =
			CollectTagContainerProperty(AbilityCDO, TEXT("AbilityTags"), TagStrings) |
			CollectTagContainerProperty(AbilityCDO, TEXT("ActivationRequiredTags"), TagStrings) |
			CollectTagContainerProperty(AbilityCDO, TEXT("ActivationBlockedTags"), TagStrings);

		if (!bFoundAny)
		{
			return false;
		}

		TagStrings.Sort();
		OutSignature = FString::Join(TagStrings, TEXT("|"));

		return true;
	}
}

EDataValidationResult UPA_AbilitySystemGenerics::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	// 按 InputID 归组
	TMap<EAbilityInputID, TArray<FString>> SignaturesByInput;
	TMap<EAbilityInputID, TArray<FString>> NamesByInput;

	for (const FGeAbilitySpecEntry& Entry : CommonAbilities)
	{
		if (!Entry.AbilityClass)
		{
			continue;
		}

		FString Signature;
		if (!MakeAbilityGateSignature(Entry, Signature))
		{
			// 读不到门控就不参与判断，避免误报
			continue;
		}

		SignaturesByInput.FindOrAdd(Entry.InputID).Add(Signature);
		NamesByInput.FindOrAdd(Entry.InputID).Add(Entry.AbilityClass->GetName());
	}

	const UEnum* InputIDEnum = StaticEnum<EAbilityInputID>();

	for (const TPair<EAbilityInputID, TArray<FString>>& Pair : SignaturesByInput)
	{
		if (Pair.Value.Num() < 2)
		{
			continue;
		}

		const TArray<FString>* Names = NamesByInput.Find(Pair.Key);
		const FString JoinedNames = Names ? FString::Join(*Names, TEXT(", ")) : FString();
		const FString InputName = InputIDEnum
			                          ? InputIDEnum->GetNameStringByValue(static_cast<int64>(Pair.Key))
			                          : FString::FromInt(static_cast<int32>(Pair.Key));

		TSet<FString> UniqueSignatures;
		int32 UngatedCount = 0;

		for (const FString& Signature : Pair.Value)
		{
			if (Signature.IsEmpty())
			{
				++UngatedCount;
			}
			UniqueSignatures.Add(Signature);
		}

		if (UniqueSignatures.Num() != Pair.Value.Num())
		{
			Context.AddError(FText::FromString(FString::Printf(
				TEXT("[%s] InputID %s 上有多条能力的门控完全相同"
					"（AbilityTags / ActivationRequiredTags / ActivationBlockedTags 一模一样），"
					"按一次输入会同时激活它们：%s"),
				*GetName(), *InputName, *JoinedNames)));

			Result = EDataValidationResult::Invalid;
		}

		if (UngatedCount > 1)
		{
			Context.AddError(FText::FromString(FString::Printf(
				TEXT("[%s] InputID %s 上有 %d 条能力完全没有门控（没有分组 Tag、也没有 Required/Blocked Tag），"
					"按一次输入会同时激活它们：%s"),
				*GetName(), *InputName, UngatedCount, *JoinedNames)));

			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif
