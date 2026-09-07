// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdCommonUserWidgetBase.h"
#include "SdWidgetProtocolReceiver.generated.h"

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetProtocolReceiver : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
	{
	}

	FDelegateHandle ClientRecvDelegate;

	virtual void BindClientRcv();
};
