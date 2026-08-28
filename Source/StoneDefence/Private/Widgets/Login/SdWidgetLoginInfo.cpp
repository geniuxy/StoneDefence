// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Login/SdWidgetLoginInfo.h"

#include "Components/EditableTextBox.h"
#include "Widgets/Components/Button/SdCommonButtonImage.h"
#include "Widgets/Login/SdWidgetLoginMenu.h"

void USdWidgetLoginInfo::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Enter->OnReleased().AddUObject(this, &ThisClass::SignIn);
	Button_Register->OnReleased().AddUObject(this, &ThisClass::Register);
}

void USdWidgetLoginInfo::NativeDestruct()
{
	Super::NativeDestruct();
}

void USdWidgetLoginInfo::SignIn()
{
	if (USdWidgetLoginMenu* LoginMenu = GetParentWidget<USdWidgetLoginMenu>())
	{
		FString AccountStr = EditableText_Account->GetText().ToString();
		FString PasswordStr = EditableText_Password->GetText().ToString();
		LoginMenu->SignIn(AccountStr, PasswordStr);
	}
}

void USdWidgetLoginInfo::Register()
{
	if (USdWidgetLoginMenu* LoginMenu = GetParentWidget<USdWidgetLoginMenu>())
	{
		LoginMenu->Register();
	}
}
