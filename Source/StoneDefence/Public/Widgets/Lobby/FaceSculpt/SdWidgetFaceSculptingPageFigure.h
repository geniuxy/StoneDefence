// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/SdCommonActivatableWidgetBase.h"
#include "SdWidgetFaceSculptingPageFigure.generated.h"

class UCommonListView;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetFaceSculptingPageFigure : public USdCommonActivatableWidgetBase
{
	GENERATED_BODY()

public:
	void ConfigurePageFigure();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnActivated() override;

private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonListView* FigureOptionListView;
	/********************/
};
