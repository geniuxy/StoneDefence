// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetFaceSculptingPageFigure.generated.h"

class UCommonListView;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetFaceSculptingPageFigure : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonListView* FigureOptionListView;
	/********************/
};
