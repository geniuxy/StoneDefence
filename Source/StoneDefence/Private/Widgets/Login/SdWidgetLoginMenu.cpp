// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Login/SdWidgetLoginMenu.h"

#include "Frameworks/GameInstance/SdGameInstance.h"

void USdWidgetLoginMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		ClientGameInstance->CreateClient();
		if (ClientGameInstance->GetClient())
		{
			ClientGameInstance->GetClient()->NetManageMsgDelegate.BindUObject(this, &ThisClass::ShowServerInfo);
			
			ClientGameInstance->LinkServer();

			BindClientRcv();
		}
	}
}

void USdWidgetLoginMenu::NativeDestruct()
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

void USdWidgetLoginMenu::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
}

void USdWidgetLoginMenu::BindClientRcv()
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

void USdWidgetLoginMenu::ShowServerInfo(ESimpleNetErrorType InType, const FString& InMsg)
{
	if (InType == HAND_SHAKE_SUCCESS)
	{
		LinkWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
