// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetFaceSculpting.generated.h"

class USdWidgetFaceSculptingPageFace;
class USdWidgetFaceSculptingPageFigure;
class USdWidgetFaceSculptingPageTalent;
class UCommonVisibilitySwitcher;
class UCommonListView;
/**
 * 捏脸界面
 */
UCLASS()
class STONEDEFENCE_API USdWidgetFaceSculpting : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonVisibilitySwitcher* Switcher;

	UPROPERTY(meta = (BindWidget))
	UCommonListView* PageListView;

	UPROPERTY(meta = (BindWidget))
	USdWidgetFaceSculptingPageFigure* PageFigureWidget;
	
	UPROPERTY(meta = (BindWidget))
	USdWidgetFaceSculptingPageFace* PageFaceWidget;
	
	UPROPERTY(meta = (BindWidget))
	USdWidgetFaceSculptingPageTalent* PageTalentWidget;
	/********************/

	void PageSelected(UObject* InSelectedObject);
};
