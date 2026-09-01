// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetCreateCharacterPanel.generated.h"

class USdCommonButtonImage;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetCreateCharacterPanel : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	/** Binding Widgets */
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* EditBox_NewName;
	
	UPROPERTY(meta=(BindWidget))
	USdCommonButtonImage* VerifyName;

	UPROPERTY(meta=(BindWidget))
	USdCommonButtonImage* Button_Create;

	UPROPERTY(meta=(BindWidget))
	USdCommonButtonImage* Button_Cancel;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* FadeIn;
	/********************/

private:
	void ButtonVerifyNameClicked();
	void ButtonCreateClicked();
	void ButtonCancelClicked();

public:
	void PanelFadeIn();
};
