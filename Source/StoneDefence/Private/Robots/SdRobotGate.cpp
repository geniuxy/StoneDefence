// Fill out your copyright notice in the Description page of Project Settings.


#include "Robots/SdRobotGate.h"

#include "SimpleNetManage.h"
#include "Global/SimpleNetGlobalInfo.h"


SdRobotGate::SdRobotGate()
{
}

SdRobotGate::~SdRobotGate()
{
	if (GateClient)
	{
		FSimpleNetManage::Destroy(GateClient);
		GateClient = nullptr;
	}
}

void SdRobotGate::Tick(float DeltaTime)
{
	if (GateClient)
	{
		GateClient->Tick(DeltaTime);
	}
}

void SdRobotGate::Init(const FString& Host, const int32 Port)
{
	FSimpleNetGlobalInfo::Get()->Init();

	GateClient = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_CONNET, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);

	GateClient->NetManageMsgDelegate.BindRaw(this, &SdRobotGate::HandleServerLinkInfo);
	if (!GateClient->Init())
	{
		delete GateClient;
		GateClient = nullptr;
	}
}

void SdRobotGate::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
}

void SdRobotGate::HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg)
{
}

void SdRobotGate::BindClientRcv()
{
}
