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

#define LOCTEXT_NAMESPACE "USdWidgetLobbyMain"

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
			HandleCharacterAppearanceResponses(Channel);
			break;
		}
	case SP_CheckCharacterNameResponses:
		{
			HandleCheckCharacterNameResponses(Channel);
			break;
		}
	case SP_CreateCharacterResponses:
		{
			HandleCreateCharacterResponses(Channel);
			break;
		}
	default:
		break;
	}
}

void USdWidgetLobbyMain::HandleCharacterAppearanceResponses(FSimpleChannel* Channel)
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
}

void USdWidgetLobbyMain::HandleCheckCharacterNameResponses(FSimpleChannel* Channel)
{
	ECheckNameType CheckNameType = UNKNOWN_ERROR;
	SIMPLE_PROTOCOLS_RECEIVE(SP_CheckCharacterNameResponses, CheckNameType);

	PrintLogByCheckName(CheckNameType);
}

void USdWidgetLobbyMain::HandleCreateCharacterResponses(FSimpleChannel* Channel)
{
	ECheckNameType CheckNameType = UNKNOWN_ERROR;
	bool bCharacterCreated = false;
	FString CAJson;
	SIMPLE_PROTOCOLS_RECEIVE(SP_CreateCharacterResponses, CheckNameType, bCharacterCreated, CAJson);

	if (bCharacterCreated)
	{
		PrintLog(LOCTEXT("CREATE_CHARACTER_RESPONSES_SUCCESSFULLY", "角色创建成功"));
		FSdCharacterAppearance InCA;
		NetDataAnalysis::StringToCharacterAppearance(CAJson, InCA);
		if (ASdPlayerStateLobby* InPlayerState = GetOwningPlayerState<ASdPlayerStateLobby>())
		{
			InPlayerState->UpdateCharacterAppearances(InCA);

			CharacterSelectionPanel->UpdateCharacterAppearances();

			CharacterSelectionPanel->SelectRecentCharacter();
		}
	}
	else
	{
		PrintLog(LOCTEXT("CREATE_CHARACTER_RESPONSES_FAIL", "角色创建失败"));
		FTimerHandle TmpTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(TmpTimeHandle, FTimerDelegate::CreateLambda([&]()
		{
			PrintLogByCheckName(CheckNameType);
		}), 1.5f, false);
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

void USdWidgetLobbyMain::PrintLogByCheckName(ECheckNameType InCheckNameType)
{
	switch (InCheckNameType)
	{
	case UNKNOWN_ERROR:
		PrintLog(LOCTEXT("CHECK_NAME_UNKNOWN_ERROR", "验证角色名字时发生未知错误"));
		break;
	case NAME_EMPTY:
		PrintLog(LOCTEXT("CHECK_NAME_NAME_EMPTY", "角色名字不能为空"));
		break;
	case NAME_NOT_EXIST:
		PrintLog(LOCTEXT("CHECK_NAME_NAME_NOT_EXIST", "当前角色名字可创建！"));
		break;
	case SERVER_NOT_EXIST:
		PrintLog(LOCTEXT("CHECK_NAME_SERVER_NOT_EXIST", "访问服务器时发生错误"));
		break;
	case NAME_EXIST:
		PrintLog(LOCTEXT("CHECK_NAME_NAME_EXIST", "角色名字已存在"));
		break;
	}
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

void USdWidgetLobbyMain::ConfigurePreviewInputCapture(AActor* InPreviewActor)
{
	if (PreviewInputCaptureWidget)
	{
		PreviewInputCaptureWidget->ConfigurePreviewInputCaptureWidget(InPreviewActor);
	}
}

void USdWidgetLobbyMain::CheckNewName(FString NewCharacterName)
{
	if (USdGameInstance* InGameInstance = GetGameInstance<USdGameInstance>())
	{
		int32 UserId = InGameInstance->GetUserData().Id;
		SEND_DATA(SP_CheckCharacterNameRequests, UserId, NewCharacterName);
	}
}

void USdWidgetLobbyMain::CreateCharacter(const FSdCharacterAppearance& InCA)
{
	if (USdGameInstance* InGameInstance = GetGameInstance<USdGameInstance>())
	{
		FString CAJson;

		NetDataAnalysis::CharacterAppearanceToString(InCA, CAJson);
		int32 UserId = InGameInstance->GetUserData().Id;

		SEND_DATA(SP_CreateCharacterRequests, UserId, CAJson);
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

#undef LOCTEXT_NAMESPACE