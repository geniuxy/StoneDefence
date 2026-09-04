// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/SdWidgetCreateCharacterPanel.h"

#include "Components/EditableTextBox.h"
#include "Frameworks/PlayerStates/SdPlayerStateLobby.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"
#include "Widgets/Components/Button/SdCommonButtonImage.h"
#include "Widgets/Lobby/SdWidgetLobbyMain.h"

class ASdPlayerStateLobby;

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
	ASdPlayerStateLobby* PlayerState = GetOwningPlayerState<ASdPlayerStateLobby>();
	if (!PlayerState) return;
	if (!PlayerState->GetCurSelectedCharacterAppearance().IsSet()) return;
	if (!PlayerState->GetCurSelectedCharacterAppearance().GetValue().IsEmpty()) return;

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
			TmpCreateCharacter.SlotIndex = USdGISubsystemLobby::Get(this)->GetCurSelectedSlotIndex();

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
