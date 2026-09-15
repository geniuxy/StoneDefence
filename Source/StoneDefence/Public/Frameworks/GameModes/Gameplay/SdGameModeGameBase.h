// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleNetChannelType.h"
#include "Frameworks/GameModes/SdGameModeBase.h"
#include "SdGameModeGameBase.generated.h"

class FSimpleChannel;
/**
 * GameMode只在Server端存在
 */
UCLASS()
class STONEDEFENCE_API ASdGameModeGameBase : public ASdGameModeBase
{
	GENERATED_BODY()

public:
	ASdGameModeGameBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	FDelegateHandle ClientRecvDelegate;
	
	virtual void BindClientRcv();

	virtual void HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg);

	void LinkServer();
};
