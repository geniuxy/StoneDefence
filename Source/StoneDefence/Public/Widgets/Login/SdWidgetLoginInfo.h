// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetLoginInfo.generated.h"

class USdCommonButtonImage;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetLoginInfo : public USdCommonUserWidgetBase
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
	USdCommonButtonImage* Button_Enter;
	
	UPROPERTY(meta=(BindWidget))
	USdCommonButtonImage* Button_Register;
	/********************/

	void SignIn();
	void Register();

	UFUNCTION()
	void OnAccountChanged(const FText& InText);

	UFUNCTION()
	void OnPasswordChanged(const FText& InText);

public:
	bool EncryptionToLocal(const FString& InPaths);
	bool DecryptionFromLocal(const FString& InPaths);

public:
	void ShowLoginWarnings();

	void ClearAccountPassword();
	
protected:
	bool CheckText(const FString& InString, int32 InMin, int32 InMax);

private:
	TMap<FString, FString> OutMap;
};
