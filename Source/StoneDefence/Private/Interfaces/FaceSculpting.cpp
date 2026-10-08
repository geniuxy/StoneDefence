// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/FaceSculpting.h"

void IFaceSculpting::UpdateFigureTypeSize(ESdFigureType InType, int32 InValue)
{
	if (FigureSizeMap.Contains(InType))
	{
		FigureSizeMap[InType] = InValue;
	}
	else
	{
		FigureSizeMap.Add(InType, InValue);
	}
}

void IFaceSculpting::UpdateFigureTypeSize(TArray<FFaceSculptFigureTypeInfo> InFigureSettings)
{
	for (const FFaceSculptFigureTypeInfo& TypeInfo : InFigureSettings)
	{
		UpdateFigureTypeSize(TypeInfo.Type, TypeInfo.CurValue);
	}
}

void IFaceSculpting::UpdateFigureTypeSizeByDefault(TArray<FFaceSculptFigureTypeInfo> InFigureSettings)
{
	for (const FFaceSculptFigureTypeInfo& TypeInfo : InFigureSettings)
	{
		UpdateFigureTypeSize(
			TypeInfo.Type,
			FMath::Clamp(TypeInfo.DefaultValue * TypeInfo.MaxValue, TypeInfo.MinValue, TypeInfo.MaxValue)
		);
	}
}

void IFaceSculpting::UpdateFigureTypeSize(const FString& InFigureSizeStr)
{
	TArray<FString> StrFigureSizeArray;
	InFigureSizeStr.ParseIntoArray(StrFigureSizeArray, TEXT("|"));
	for (const FString& StrFigureSize : StrFigureSizeArray)
	{
		TArray<FString> StrFigureTypeAndSize;
		StrFigureSize.ParseIntoArray(StrFigureTypeAndSize, TEXT(","));
		if (StrFigureTypeAndSize.Num() != 2) continue;

		int TypeIndex = FCString::Atoi(*StrFigureTypeAndSize[0]);
		if (TypeIndex >= 0 && TypeIndex < static_cast<int>(ESdFigureType::FT_NUM))
		{
			UpdateFigureTypeSize(static_cast<ESdFigureType>(TypeIndex), FCString::Atoi(*StrFigureTypeAndSize[1]));
		}
	}
}

int32 IFaceSculpting::GetFigureSizeByType(ESdFigureType InType)
{
	// return FigureSizeMap.Contains(InType) ? FigureSizeMap[InType] : 0;
	return FigureSizeMap.FindRef(InType, 0);
}
