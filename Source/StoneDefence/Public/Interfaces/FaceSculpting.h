// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "UObject/Interface.h"
#include "FaceSculpting.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFaceSculpting : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STONEDEFENCE_API IFaceSculpting
{
	GENERATED_BODY()

public:
	void UpdateFigureTypeSize(ESdFigureType InType, int32 InValue);
	void UpdateFigureTypeSize(TArray<FFaceSculptFigureTypeInfo> InFigureSettings);
	void UpdateFigureTypeSizeByDefault(TArray<FFaceSculptFigureTypeInfo> InFigureSettings);
	void UpdateFigureTypeSize(const FString& InFigureSizeStr);
	int32 GetFigureSizeByType(ESdFigureType InType);
	
protected:
	TMap<ESdFigureType, int32> FigureSizeMap;
};
