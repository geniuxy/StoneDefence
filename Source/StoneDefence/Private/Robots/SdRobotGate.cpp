// Fill out your copyright notice in the Description page of Project Settings.


#include "Robots/SdRobotGate.h"

#include "SimpleNetManage.h"
#include "Global/SimpleNetGlobalInfo.h"
#include "Protocol/LobbyProtocol.h"
#include "SdTypes/SdMacros.h"

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

	GateClient = FSimpleNetManage::CreateManage(
		ESimpleNetLinkState::LINKSTATE_CONNET, ESimpleSocketType::SIMPLESOCKETTYPE_TCP
	);

	GateClient->NetManageMsgDelegate.BindRaw(this, &SdRobotGate::HandleServerLinkInfo);
	if (!GateClient->Init(Host, Port))
	{
		delete GateClient;
		GateClient = nullptr;
		return;
	}

	GateClient->GetController()->RecvDelegate.AddLambda(
		[&](uint32 ProtocolNumber, FSimpleChannel* Channel)
		{
			RecvProtocol(ProtocolNumber, Channel);
		}
	);
}

void SdRobotGate::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch (ProtocolNumber)
	{
	case SP_CharacterAppearanceResponses:
		{
			HandleCharacterAppearanceResponses(Channel);
			break;
		}
	case SP_LoginToDsServerResponses:
		{
			HandleLoginToDsServerResponses(Channel);
			break;
		}
	default:
		break;
	}
}

void SdRobotGate::HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg)
{
	if (InType == HAND_SHAKE_SUCCESS)
	{
		RunCharacterAppearanceRequests();
	}
}

void SdRobotGate::BindClientRcv()
{
}

void SdRobotGate::RunCharacterAppearanceRequests()
{
	int32 Id = SD_ROBOT_USERID;
	SIMPLE_CLIENT_SEND(GateClient, SP_CharacterAppearanceRequests, Id);
}

void SdRobotGate::HandleCharacterAppearanceResponses(FSimpleChannel* Channel)
{
	FString CharacterJson;

	SIMPLE_PROTOCOLS_RECEIVE(SP_CharacterAppearanceResponses, CharacterJson);

	if (!CharacterJson.IsEmpty())
	{
		RunLoginToDsServerRequests();
	}
}

void SdRobotGate::RunLoginToDsServerRequests()
{
	int32 Id = SD_ROBOT_USERID;
	int32 SlotId = 3;
	SIMPLE_CLIENT_SEND(GateClient, SP_LoginToDsServerRequests, Id, SlotId);
}

void SdRobotGate::HandleLoginToDsServerResponses(FSimpleChannel* Channel)
{
	FSimpleAddr DSAddr;
	SIMPLE_PROTOCOLS_RECEIVE(SP_LoginToDsServerResponses, DSAddr);

	FString DSAddrStr = FSimpleNetManage::GetAddrString(DSAddr);

	StartDelegate.ExecuteIfBound();
}
