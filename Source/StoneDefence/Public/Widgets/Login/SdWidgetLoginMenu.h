// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleNetChannelType.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetLoginMenu.generated.h"

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetLoginMenu : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel) override;

private:
	/** Binding Widgets */
	UPROPERTY(meta=(BindWidget))
	USdCommonUserWidgetBase* LinkWidget;
	/********************/
	
	FDelegateHandle ClientRecvDelegate;
	void BindClientRcv();

	void ShowServerInfo(ESimpleNetErrorType InType, const FString& InMsg);
};
