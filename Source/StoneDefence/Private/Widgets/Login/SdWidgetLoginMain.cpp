// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Login/SdWidgetLoginMain.h"

#include "Frameworks/GameInstance/SdGameInstance.h"
#include "Widgets/Login/SdWidgetLoginInfo.h"
#include "Protocol/LoginProtocol.h"
#include "SdTypes/SdMacros.h"
#include "StoneDefenceNetCommonType.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Common/SdWidgetPrintMsg.h"
#include "Widgets/Login/SdWidgetRegisterInfo.h"

#define LOCTEXT_NAMESPACE "USdWidgetLoginMain"

void USdWidgetLoginMain::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(LoginInfoFadeIn);

	LoginInfo->SetParentWidget(this);
	if (RegisterInfo)
	{
		RegisterInfo->SetParentWidget(this);
	}

	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		ClientGameInstance->CreateClient();
		if (ClientGameInstance->GetClient())
		{
			ClientGameInstance->GetClient()->NetManageMsgDelegate.BindUObject(this, &ThisClass::ShowServerLinkingInfo);

			ClientGameInstance->LinkServer();

			BindClientRcv();
		}
	}

	if (!LoginInfo->DecryptionFromLocal(FPaths::ProjectDir() / TEXT("User")))
	{
		PrintLog(TEXT("读取密码失败..."));
	}
}

void USdWidgetLoginMain::NativeDestruct()
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

void USdWidgetLoginMain::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch (ProtocolNumber)
	{
	case SP_LoginResponses:
		{
			HandleLoginResponses(Channel);
			break;
		}
	case SP_RegisterResponses:
		{
			HandleRegisterResponses(Channel);
			break;
		}
	default:
		break;
	}
}

void USdWidgetLoginMain::SignIn(FString InAccount, FString InPassword)
{
	SEND_DATA(SP_LoginRequests, InAccount, InPassword);
}

void USdWidgetLoginMain::Register()
{
	RegisterInfo->RegisterIn();
}

void USdWidgetLoginMain::Register(FString InRegisterInfo)
{
	
}

void USdWidgetLoginMain::PrintLog(const FString& InMsg)
{
	PrintLog(FText::FromString(InMsg));
}

void USdWidgetLoginMain::PrintLog(const FText& InMsg)
{
	MsgLogWidget->PlayShowMsgAnim();
	MsgLogWidget->SetLogText(InMsg);
}

void USdWidgetLoginMain::BindClientRcv()
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

void USdWidgetLoginMain::ShowServerLinkingInfo(ESimpleNetErrorType InType, const FString& InMsg)
{
	if (InType == HAND_SHAKE_SUCCESS)
	{
		LinkWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USdWidgetLoginMain::HandleLoginResponses(FSimpleChannel* Channel)
{
	FString Msg;
	ELoginType Type = LOGIN_DB_SERVER_ERROR;
	FSdGateStatus GateStatus;
	SIMPLE_PROTOCOLS_RECEIVE(SP_LoginResponses, Type, Msg, GateStatus);

	switch (Type)
	{
	case LOGIN_SUCCESS:
		{
			if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
			{
				NetDataAnalysis::StringToUserData(Msg, ClientGameInstance->GetUserData());

				if (GateStatus.GateConnectionNum == INDEX_NONE)
				{
					PrintLog(TEXT("当前服务器已满..."));
				}
				else
				{
					ClientGameInstance->SetGateStatus(GateStatus);
							
					if (!LoginInfo->EncryptionToLocal(FPaths::ProjectDir() / TEXT("User")))
					{
						PrintLog(TEXT("存储密码失败..."));
					}
					else
					{
						PrintLog(TEXT("登录成功！！！"));
					}

					PlayAnimation(LoginInfoFadeOut);

					// 关闭LoginServer
					if (ClientGameInstance->GetClient() && ClientGameInstance->GetClient()->GetChannel())
					{
						ClientGameInstance->GetClient()->GetChannel()->DestroySelf();
					}

					// 延迟跳转至大厅
					FTimerHandle TmpTimeHandle;
					GetWorld()->GetTimerManager().SetTimer(TmpTimeHandle, FTimerDelegate::CreateLambda([this]()
					{
						UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyMap"));
					}), 2.f, false);
				}
			}
			break;
		}
	case LOGIN_DB_SERVER_ERROR:
		LoginInfo->ShowLoginWarnings();
		PrintLog(TEXT("服务器错误..."));
		break;
	case LOGIN_ACCOUNT_WRONG:
		LoginInfo->ShowLoginWarnings();
		PrintLog(TEXT("账号为空..."));
		break;
	case LOGIN_WRONG_PASSWORD:
		LoginInfo->ShowLoginWarnings();
		PrintLog(TEXT("密码错误..."));
		break;
	}
}

void USdWidgetLoginMain::HandleRegisterResponses(FSimpleChannel* Channel)
{
	ERegistrationType Type = SERVER_BUG_WRONG;

	//拿到客户端发送的账号
	SIMPLE_PROTOCOLS_RECEIVE(SP_RegisterResponses, Type);

	switch (Type)
	{
	case ACCOUNT_AND_EMAIL_REPETITION_ERROR:
		{
			PrintLog(LOCTEXT("ACCOUNT_AND_EMAIL_REPETITION_ERROR", "Duplicate account or email."));

			FTimerHandle TmpTimeHandle;
			GetWorld()->GetTimerManager().SetTimer(TmpTimeHandle, FTimerDelegate::CreateLambda([this]()
			{
				Register();

				//显示重复警告
				RegisterInfo->ShowDuplicateWarnings();
				RegisterInfo->ShowFailedRegisterWarnings();
			}), 0.8f, false);

			break;
		}
	case PLAYER_REGISTRATION_SUCCESS:
		{
			PrintLog(LOCTEXT("REGISTRATION_SUCCESS", "Registration was successful."));

			//清除原来账户信息
			LoginInfo->ClearAccountPassword();
			break;
		}
	case SERVER_BUG_WRONG:
		{
			PrintLog(LOCTEXT("SERVER_BUG_WRONG", "Server unknown error."));
			break;
		}
	default:
		break;
	}
}

#undef LOCTEXT_NAMESPACE