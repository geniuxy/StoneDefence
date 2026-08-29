// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Login/SdWidgetLoginMenu.h"

#include "CommonTextBlock.h"
#include "Frameworks/GameInstance/SdGameInstance.h"
#include "Widgets/Login/SdWidgetLoginInfo.h"
#include "Protocol/LoginProtocol.h"
#include "SdTypes/SdMacros.h"
#include "StoneDefenceNetCommonType.h"

void USdWidgetLoginMenu::NativeConstruct()
{
	Super::NativeConstruct();

	LoginInfo->SetParentWidget(this);

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

	if (!LoginInfo->DecryptionFromLocal(FPaths::ProjectDir() / TEXT("User")))
	{
		PrintLog(TEXT("读取密码失败..."));
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
	switch (ProtocolNumber)
	{
	case SP_LoginResponses:
		{
			FString Msg;
			ELoginType Type = LOGIN_DB_SERVER_ERROR;

			SIMPLE_PROTOCOLS_RECEIVE(SP_LoginResponses, Type, Msg, CachedGateStatus);

			switch (Type)
			{
			case LOGIN_SUCCESS:
				{
					if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
					{
						NetDataAnalysis::StringToUserData(Msg, ClientGameInstance->GetUserData());
					}

					if (!LoginInfo->EncryptionToLocal(FPaths::ProjectDir() / TEXT("User")))
					{
						PrintLog(TEXT("存储密码失败..."));
					}
					else
					{
						PrintLog(TEXT("登录成功！！！"));
					}
					break;
				}
			case LOGIN_DB_SERVER_ERROR:
				PrintLog(TEXT("服务器错误..."));
				break;
			case LOGIN_ACCOUNT_WRONG:
				PrintLog(TEXT("账号为空..."));
				break;
			case LOGIN_WRONG_PASSWORD:
				PrintLog(TEXT("密码错误..."));
				break;
			}
			break;
		}
	}
}

void USdWidgetLoginMenu::SignIn(FString InAccount, FString InPassword)
{
	SEND_DATA(SP_LoginRequests, InAccount, InPassword);
}

void USdWidgetLoginMenu::Register()
{
}

void USdWidgetLoginMenu::PrintLog(const FString& InMsg)
{
	PrintLog(FText::FromString(InMsg));
}

void USdWidgetLoginMenu::PrintLog(const FText& InMsg)
{
	MsgLog->SetText(InMsg);
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
