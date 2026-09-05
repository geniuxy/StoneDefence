// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "StoneDefence/StoneDefence.h"
#include "Widgets/Components/ListEntryDataObjects/SdListEntryDataObjectBase.h"
#include "Widgets/Components/ListEntryWidgets/SdWidgetListEntryBase.h"
#include "SdListEntryFaceSculptingFigure.generated.h"

class UCommonTextBlock;
class UAnalogSlider;

UCLASS()
class STONEDEFENCE_API UFaceSculptingFigureData : public USdListEntryDataObjectBase
{
	GENERATED_BODY()

	UPROPERTY()
	ESdFigureType Type;

	UPROPERTY()
	float DefaultValue; // 0~1之间

	UPROPERTY()
	int MaxValue;

	UPROPERTY()
	float CurValue;

public:
	DATA_ACCESSOR(ESdFigureType, Type)
	DATA_ACCESSOR(float, DefaultValue)
	DATA_ACCESSOR(int, MaxValue)
	DATA_ACCESSOR(float, CurValue)
};
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdListEntryFaceSculptingFigure : public USdWidgetListEntryBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* FigureTypeText;

	UPROPERTY(meta = (BindWidget))
	UAnalogSlider* AnalogSlider_Setting;
	
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* ValueText;
	/********************/

	UPROPERTY()
	UFaceSculptingFigureData* FaceSculptingFigureData;

	UFUNCTION()
	void OnSliderChanged(float InValue);
};
