// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/FaceSculpt/SdListEntryFaceSculptingFigure.h"

#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "FunctionLibraries/SdFunctionLibraryCommon.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"

void USdListEntryFaceSculptingFigure::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	FaceSculptingFigureData = Cast<UFaceSculptingFigureData>(ListItemObject);
	if (FaceSculptingFigureData)
	{
		FigureTypeText->SetText(USdFunctionLibraryCommon::GetDisplayValueOfEnum(FaceSculptingFigureData->GetType()));
		AnalogSlider_Setting->SetMaxValue(FaceSculptingFigureData->GetMaxValue());
		AnalogSlider_Setting->SetStepSize(1);
		AnalogSlider_Setting->SetValue(FaceSculptingFigureData->GetCurValue());
		AnalogSlider_Setting->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnSliderChanged);
		int32 CurValue = FMath::RoundToInt(FaceSculptingFigureData->GetCurValue());
		ValueText->SetText(FText::FromString(FString::Printf(TEXT("%02d"), CurValue)));
	}
}

void USdListEntryFaceSculptingFigure::OnSliderChanged(float InValue)
{
	int32 CurValue = FMath::RoundToInt(InValue);
	ValueText->SetText(FText::FromString(FString::Printf(TEXT("%02d"), CurValue)));

	USdGISubsystemLobby::Get(this)->UpdateCachedFigureSettings(FaceSculptingFigureData->GetType(), CurValue);
}
