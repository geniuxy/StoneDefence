// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/SdWidgetCreateCharacterPanel.h"

#include "Components/EditableTextBox.h"
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
	if (USdWidgetLobbyMain* LobbyMain = GetParentWidget<USdWidgetLobbyMain>())
	{
		FString NewNameStr = EditBox_NewName->GetText().ToString();
		LobbyMain->CheckNewName(NewNameStr);
	}
}

void USdWidgetCreateCharacterPanel::ButtonCreateClicked()
{
	if (CurSlotIndex == INDEX_NONE) return;

	if (USdWidgetLobbyMain* LobbyMain = GetParentWidget<USdWidgetLobbyMain>())
	{
		if (EditBox_NewName->GetText().IsEmpty())
		{
			LobbyMain->PrintLog(TEXT("名字不能为空..."));
		}
		else
		{
			FSdCharacterAppearance TmpCreateCharacter = FSdCharacterAppearance();
			TmpCreateCharacter.Name = EditBox_NewName->GetText().ToString();
			TmpCreateCharacter.DisplayAssetName = TEXT("PA_CharacterDefinition_Tubaki");
			TmpCreateCharacter.LastLoginTime = FDateTime::Now().ToString();
			TmpCreateCharacter.Level = 1;
			TmpCreateCharacter.SlotIndex = CurSlotIndex;

			LobbyMain->CreateCharacter(TmpCreateCharacter);
		}
	}
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
