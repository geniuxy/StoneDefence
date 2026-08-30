// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetRegisterInfo.generated.h"

class UCommonTextBlock;
class USdCommonButtonImage;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetRegisterInfo : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	/** Binding Widgets */
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* EditableText_Account;

	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* EditableText_Password;

	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* EditableText_ConfirmPassword;
	
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* EditableText_NiceName;

	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* EditableText_Email;

	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* EditableText_AuthorURL;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* NiceNameText;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* EmailText;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* PlayerAccountText;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* PlayerPasswordText;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* ConfirmPasswordText;

	UPROPERTY(meta=(BindWidget))
	USdCommonButtonImage* Button_Submit;

	UPROPERTY(meta=(BindWidget))
	USdCommonButtonImage* Button_Cancel;
	/********************/

	void Submit();
	void Cancel();

	void ResetText();
	void ResetColor();

	bool IsSpecialCharacters(const FString& InString);

public:
	void RegisterIn();
	void RegisterOut();

	void ShowFailedRegisterWarnings();

	void ShowNiceNameWarnings();
	void ShowAccountWarnings();
	void ShowEmailWarnings();
	void ShowPasswordWarnings();
	void ShowConfirmPasswordWarnings();
	void ShowDuplicateWarnings();
};
