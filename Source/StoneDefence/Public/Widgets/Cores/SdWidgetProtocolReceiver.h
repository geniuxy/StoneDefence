// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdCommonActivatableWidgetBase.h"
#include "SimpleNetChannelType.h"
#include "SdWidgetProtocolReceiver.generated.h"

class FSimpleChannel;
class USdWidgetPrintMsg;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetProtocolReceiver : public USdCommonActivatableWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void PrintLog(const FString& InMsg);
	void PrintLog(const FText& InMsg);

protected:
	UPROPERTY(meta=(BindWidget))
	USdWidgetPrintMsg* MsgLogWidget;

protected:
	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
	{
	}

	FDelegateHandle ClientRecvDelegate;

	virtual void BindClientRcv();

	virtual void HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg)
	{
	}
	
	void LinkServer();
	void LinkServer(const FSimpleAddr& InAddr);
};
