// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/GeCommonUserWidgetBase.h"
#include "SdWidgetCreateCharacterPanel.generated.h"

class UGeCommonButtonImage;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetCreateCharacterPanel : public UGeCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	/** Binding Widgets */
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* EditBox_NewName;

	UPROPERTY(meta=(BindWidget))
	UGeCommonButtonImage* VerifyName;

	UPROPERTY(meta=(BindWidget))
	UGeCommonButtonImage* Button_Create;

	UPROPERTY(meta=(BindWidget))
	UGeCommonButtonImage* Button_Cancel;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* FadeIn;
	/********************/

private:
	void ButtonVerifyNameClicked();
	void ButtonCreateClicked();
	void ButtonCancelClicked();

public:
	void PanelFadeIn();
	void HidePanel();

	void SetButtonCreateText(FText InText);
	void SetEditNewNameText(FText InText);
};
