// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Cores/SdWidgetProtocolReceiver.h"

#include "Frameworks/GameInstance/SdGameInstance.h"
#include "Widgets/Common/SdWidgetPrintMsg.h"

void USdWidgetProtocolReceiver::NativeConstruct()
{
	Super::NativeConstruct();
}

void USdWidgetProtocolReceiver::NativeDestruct()
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

void USdWidgetProtocolReceiver::PrintLog(const FString& InMsg)
{
	PrintLog(FText::FromString(InMsg));
}

void USdWidgetProtocolReceiver::PrintLog(const FText& InMsg)
{
	MsgLogWidget->PlayShowMsgAnim();
	MsgLogWidget->SetLogText(InMsg);
}

void USdWidgetProtocolReceiver::BindClientRcv()
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

void USdWidgetProtocolReceiver::LinkServer()
{
	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		ClientGameInstance->CreateClient();
		if (ClientGameInstance->GetClient())
		{
			ClientGameInstance->GetClient()->NetManageMsgDelegate.BindUObject(this, &ThisClass::HandleServerLinkInfo);

			ClientGameInstance->LinkServer();

			BindClientRcv();
		}
	}
}

void USdWidgetProtocolReceiver::LinkServer(const FSimpleAddr& InAddr)
{
	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		ClientGameInstance->CreateClient();
		if (ClientGameInstance->GetClient())
		{
			ClientGameInstance->GetClient()->NetManageMsgDelegate.BindUObject(this, &ThisClass::HandleServerLinkInfo);

			ClientGameInstance->LinkServer(InAddr);

			BindClientRcv();
		}
	}
}
