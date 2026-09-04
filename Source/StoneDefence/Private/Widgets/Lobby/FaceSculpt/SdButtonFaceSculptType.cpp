// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/FaceSculpt/SdButtonFaceSculptType.h"

#include "FunctionLibraries/SdFunctionLibraryCommon.h"

void USdButtonFaceSculptType::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	FaceSculptPageData = Cast<UFaceSculptPageData>(ListItemObject);
	if (FaceSculptPageData)
	{
		SetButtonText(USdFunctionLibraryCommon::GetDisplayValueOfEnum(FaceSculptPageData->GetType()));
	}
}
