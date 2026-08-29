// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GameInstance/SdGameInstance.h"
#include "Global/SimpleNetGlobalInfo.h"

void USdGameInstance::Init()
{
	Super::Init();
}

void USdGameInstance::Tick(float DeltaTime)
{
	if (Client)
	{
		Client->Tick(DeltaTime);
	}
}

TStatId USdGameInstance::GetStatId() const
{
	return TStatId();
}

void USdGameInstance::Shutdown()
{
	Super::Shutdown();

	if (Client)
	{
		FSimpleNetManage::Destroy(Client);
	}
}

void USdGameInstance::CreateClient()
{
	FSimpleNetGlobalInfo::Get()->Init();

	Client = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_CONNET, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);
}

void USdGameInstance::LinkServer()
{
	if (Client)
	{
		if (!Client->Init())
		{
			delete Client;
			Client = nullptr;
		}
	}
}

FSimpleNetManage* USdGameInstance::GetClient() const
{
	return Client;
}

FSdUserData& USdGameInstance::GetUserData()
{
	return UserData;
}
