#pragma once
#include "CoreMinimal.h"
#include "Internationalization/Text.h"

template<typename TEnum>
struct TEnumDisplayRegistrar
{
	static TMap<int32, FText> DisplayMap;

	static FText Callback(int32 Index)
	{
		if (const FText* Found = DisplayMap.Find(Index))
		{
			return *Found;
		}
		return FText::GetEmpty();
	}

	static void Add(TEnum Value, const FText& Text)
	{
		DisplayMap.Emplace(static_cast<int32>(Value), Text);
	}

	static void Register()
	{
		UEnum* Enum = StaticEnum<TEnum>();
		if (!Enum) return;

		// 直接传入静态函数，完美匹配 FEnumDisplayNameFn 函数指针
		Enum->SetEnumDisplayNameFn(&TEnumDisplayRegistrar<TEnum>::Callback);
	}
};

template<typename TEnum>
TMap<int32, FText> TEnumDisplayRegistrar<TEnum>::DisplayMap;
