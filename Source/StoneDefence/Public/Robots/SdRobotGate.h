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
	FSimpleDelegate StartDelegate;
	
	SdRobotGate();
	virtual ~SdRobotGate();

	void Tick(float DeltaTime);
	
	void Init(const FString& Host, const int32 Port);

	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	virtual void HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg);

	virtual void BindClientRcv();

	// 为了在网关注册Id
	void RunCharacterAppearanceRequests();
	// 为了在服务器注册一个玩家
	void HandleCharacterAppearanceResponses(FSimpleChannel* Channel);
	
	void RunLoginToDsServerRequests();
	void HandleLoginToDsServerResponses(FSimpleChannel* Channel);

private:
	FSimpleNetManage* GateClient = nullptr;
};
