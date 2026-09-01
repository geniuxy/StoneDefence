// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/SdWidgetCreateCharacterPanel.h"

#include "Widgets/Components/Button/SdCommonButtonImage.h"
#include "Widgets/Lobby/SdWidgetLobbyMain.h"

void USdWidgetCreateCharacterPanel::NativeConstruct()
{
	Super::NativeConstruct();

	VerifyName->OnReleased().AddUObject(this, &ThisClass::ButtonVerifyNameClicked);
	Button_Create->OnReleased().AddUObject(this, &ThisClass::ButtonCreateClicked);
	Button_Cancel->OnReleased().AddUObject(this, &ThisClass::ButtonCancelClicked);
}

void USdWidgetCreateCharacterPanel::ButtonVerifyNameClicked()
{
}

void USdWidgetCreateCharacterPanel::ButtonCreateClicked()
{
	SetRenderOpacity(0.f);
}

void USdWidgetCreateCharacterPanel::ButtonCancelClicked()
{
	SetRenderOpacity(0.f);
	if (USdWidgetLobbyMain* LobbyMain = GetParentWidget<USdWidgetLobbyMain>())
	{
		LobbyMain->BackToCharacterSelectionPanel();
	}
}

void USdWidgetCreateCharacterPanel::PanelFadeIn()
{
	PlayAnimation(FadeIn);
}
