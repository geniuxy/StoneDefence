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
	HidePanel();
}

void USdWidgetCreateCharacterPanel::ButtonCancelClicked()
{
	HidePanel();
	if (USdWidgetLobbyMain* LobbyMain = GetParentWidget<USdWidgetLobbyMain>())
	{
		LobbyMain->BackToCharacterSelectionPanel();
		LobbyMain->SelectRecentCharacter();
	}
}

void USdWidgetCreateCharacterPanel::PanelFadeIn()
{
	PlayAnimation(FadeIn);
}

void USdWidgetCreateCharacterPanel::HidePanel()
{
	SetRenderOpacity(0.f);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}
