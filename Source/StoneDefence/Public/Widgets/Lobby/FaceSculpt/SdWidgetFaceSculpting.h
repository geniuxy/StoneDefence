// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/SdCommonActivatableWidgetBase.h"
#include "SdWidgetFaceSculpting.generated.h"

class UCommonActivatableWidgetSwitcher;
class USdWidgetFaceSculptingPageFace;
class USdWidgetFaceSculptingPageFigure;
class USdWidgetFaceSculptingPageTalent;
class UCommonVisibilitySwitcher;
class UCommonListView;
/**
 * 捏脸界面
 */
UCLASS()
class STONEDEFENCE_API USdWidgetFaceSculpting : public USdCommonActivatableWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnActivated() override;

private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetSwitcher* Switcher;

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
