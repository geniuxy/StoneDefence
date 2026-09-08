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
	if (!USdGISubsystemLobby::Get(this)) return;
	ASdPlayerStateLobby* PlayerState = GetOwningPlayerState<ASdPlayerStateLobby>();
	if (!PlayerState) return;
	bool bIsCreatingCharacter = !PlayerState->GetCurSelectedCharacterAppearance().IsSet();

	USdGISubsystemLobby* LobbySubsystem = USdGISubsystemLobby::Get(this);
	if (!LobbySubsystem) return;
	USdWidgetLobbyMain* LobbyMain = GetParentWidget<USdWidgetLobbyMain>();
	if (!LobbyMain) return;

	if (EditBox_NewName->GetText().IsEmpty())
	{
		LobbyMain->PrintLog(TEXT("名字不能为空..."));
	}
	else
	{
		FSdCharacterAppearance TmpCreateCharacter = FSdCharacterAppearance();
		TmpCreateCharacter.Name = EditBox_NewName->GetText().ToString();
		TmpCreateCharacter.DisplayAssetName = LobbySubsystem->GetCurSelectedCharacterDefinition()->GetName();
		TmpCreateCharacter.LastLoginTime = FDateTime::Now().ToString();
		TmpCreateCharacter.Level = 1;
		TmpCreateCharacter.SlotIndex = USdGISubsystemLobby::Get(this)->GetCurSelectedSlotIndex();

		FString FigureSizeStr;
		for (FFaceSculptFigureTypeInfo CachedInfo : LobbySubsystem->GetCachedFigureSettings())
		{
			FigureSizeStr += FString::FromInt(static_cast<int>(CachedInfo.Type)) + TEXT(",") +
				FString::FromInt(CachedInfo.GetCurValue()) + TEXT("|");
		}
		FigureSizeStr.RemoveFromEnd(TEXT("|"));
		TmpCreateCharacter.FigureSizeStr = FigureSizeStr;

		if (bIsCreatingCharacter)
		{
			LobbyMain->CreateCharacter(TmpCreateCharacter);
		}
		else
		{
			LobbyMain->EditCharacter(TmpCreateCharacter);
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

	if (USdGISubsystemLobby* LobbySubsystem = USdGISubsystemLobby::Get(this))
	{
		if (LobbySubsystem->GetIsEditingCharacter())
		{
			LobbySubsystem->SetIsEditingCharacter(false);
			LobbySubsystem->SetCachedSlotIndex(INDEX_NONE); // 如果是在编辑角色的话，取消之后需要去恢复服务端上的设置项
		}
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

void USdWidgetCreateCharacterPanel::SetButtonCreateText(FText InText)
{
	Button_Create->SetButtonText(InText, ESdTextJustify::Center);
}

void USdWidgetCreateCharacterPanel::SetEditNewNameText(FText InText)
{
	EditBox_NewName->SetText(InText);
}
