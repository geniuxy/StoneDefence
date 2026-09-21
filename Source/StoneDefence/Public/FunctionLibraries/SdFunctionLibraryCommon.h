// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SdFunctionLibraryCommon.generated.h"

enum class ESdFigureType : uint8;
enum class ESdFaceSculptPageType : uint8;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdFunctionLibraryCommon : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template <typename EnumType>
	static FString GetStringValueOfEnum(EnumType InEnumType)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();

		return StaticEnumOption->GetNameStringByIndex(static_cast<int64>(InEnumType));
	}

	// Tips: 这个方法不能用于打包出来的版本
	template <typename EnumType>
	static FText GetDisplayValueOfEnum(EnumType InEnumType)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		
		return StaticEnumOption->GetDisplayNameTextByIndex(static_cast<int64>(InEnumType));
	}
};
