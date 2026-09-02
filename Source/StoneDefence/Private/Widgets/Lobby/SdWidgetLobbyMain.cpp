// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/SdWidgetLobbyMain.h"

#include "Frameworks/GameInstance/SdGameInstance.h"
#include "Frameworks/PlayerStates/SdPlayerStateLobby.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Common/SdWidgetPrintMsg.h"
#include "Widgets/Lobby/SdWidgetCharacterSelectionPanel.h"
#include "Widgets/Lobby/SdWidgetCreateCharacterPanel.h"
#include "Protocol/LobbyProtocol.h"
#include "SdTypes/SdMacros.h"
#include "Widgets/Common/SdWidgetPreviewInputCapture.h"
#include "Widgets/Components/Button/SdCommonButtonImage.h"


void USdWidgetLobbyMain::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterSelectionPanel->SetParentWidget(this);
	CreateCharacterPanel->SetParentWidget(this);
	PreviewInputCaptureWidget->SetParentWidget(this);
	CreateCharacterPanel->HidePanel();

	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		if (ClientGameInstance->GetClient())
		{
			ClientGameInstance->GetClient()->NetManageMsgDelegate.BindUObject(this, &ThisClass::HandleServerLinkInfo);
			// 这一步连接到GateServer
			ClientGameInstance->GetClient()->Init(ClientGameInstance->GetGateStatus().GateServerAddrInfo.Addr);

			BindClientRcv();
		}
	}

	Button_BeginGame->OnReleased().AddUObject(this, &ThisClass::BeginGame);
}

void USdWidgetLobbyMain::NativeDestruct()
{
	Super::NativeDestruct();

	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		if (ClientGameInstance->GetClient() && ClientGameInstance->GetClient()->GetController())
		{
			ClientGameInstance->GetClient()->GetController()->RecvDelegate.Remove(ClientRecvDelegate);
		}
	}
}

void USdWidgetLobbyMain::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch (ProtocolNumber)
	{
	case SP_CharacterAppearanceResponses:
		{
			FString CharacterJson;

			SIMPLE_PROTOCOLS_RECEIVE(SP_CharacterAppearanceResponses, CharacterJson);

			if (!CharacterJson.IsEmpty())
			{
				if (ASdPlayerStateLobby* PlayerState = GetOwningPlayerState<ASdPlayerStateLobby>())
				{
					NetDataAnalysis::StringToCharacterAppearances(
						CharacterJson, PlayerState->GetCachedCharacterAppearances()
					);

					CharacterSelectionPanel->UpdateCharacterAppearances();

					CharacterSelectionPanel->SelectRecentCharacter();
				}
			}

			break;
		}
	default:
		break;
	}
}

void USdWidgetLobbyMain::PrintLog(const FString& InMsg)
{
	PrintLog(FText::FromString(InMsg));
}

void USdWidgetLobbyMain::PrintLog(const FText& InMsg)
{
	MsgLogWidget->PlayShowMsgAnim();
	MsgLogWidget->SetLogText(InMsg);
}

void USdWidgetLobbyMain::BackToCharacterSelectionPanel()
{
	CharacterSelectionPanel->BackToCharacterSelectionPanel();
}

void USdWidgetLobbyMain::SelectRecentCharacter()
{
	CharacterSelectionPanel->SelectRecentCharacter();
}

void USdWidgetLobbyMain::HandleSelectCharacterSlot(bool bCreateCharacter)
{
	if (bCreateCharacter)
	{
		CreateCharacterPanel->PanelFadeIn();
		Button_BeginGame->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		CreateCharacterPanel->HidePanel();
		Button_BeginGame->SetVisibility(ESlateVisibility::Visible);
	}
}

void USdWidgetLobbyMain::ConfigurePreviewInputCapture(AActor* InDisplayActor)
{
	if (PreviewInputCaptureWidget)
	{
		PreviewInputCaptureWidget->ConfigurePreviewInputCaptureWidget(InDisplayActor);
	}
}

void USdWidgetLobbyMain::BindClientRcv()
{
	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		if (ClientGameInstance->GetClient() && ClientGameInstance->GetClient()->GetController())
		{
			ClientRecvDelegate = ClientGameInstance->GetClient()->GetController()->RecvDelegate.AddLambda(
				[&](uint32 ProtocolNumber, FSimpleChannel* Channel)
				{
					this->RecvProtocol(ProtocolNumber, Channel);
				}
			);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::BindClientRcv);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::BindClientRcv);
	}
}

void USdWidgetLobbyMain::HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg)
{
	if (InType == HAND_SHAKE_SUCCESS)
	{
		SEND_DATA(SP_CharacterAppearanceRequests, ClientGameInstance->GetUserData().Id)
	}
}

void USdWidgetLobbyMain::BeginGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("GameMap"));
}
