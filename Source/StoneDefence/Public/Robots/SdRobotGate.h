// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Channel/SimpleChannel.h"

class FSimpleNetManage;
/**
 * 
 */
class STONEDEFENCE_API SdRobotGate
{
public:
	SdRobotGate();
	virtual ~SdRobotGate();

	void Tick(float DeltaTime);
	
	void Init(const FString& Host, const int32 Port);

	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	virtual void HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg);

	virtual void BindClientRcv();

private:
	FSimpleNetManage* GateClient = nullptr;
};
