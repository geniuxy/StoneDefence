// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/SdWidgetLobbyMain.h"

#include "Frameworks/GameInstance/SdGameInstance.h"
#include "Widgets/Common/SdWidgetPrintMsg.h"


void USdWidgetLobbyMain::NativeConstruct()
{
	Super::NativeConstruct();

	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		if (ClientGameInstance->GetClient())
		{
			// 这一步连接到GateServer
			ClientGameInstance->GetClient()->Init(ClientGameInstance->GetGateStatus().GateServerAddrInfo.Addr);

			BindClientRcv();
		}
	}
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
	Super::RecvProtocol(ProtocolNumber, Channel);
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
