// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleNetChannelType.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetLobbyMain.generated.h"

class USdWidgetCharacterSelectionPanel;
class USdWidgetPrintMsg;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetLobbyMain : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel) override;

public:
	void PrintLog(const FString& InMsg);
	void PrintLog(const FText& InMsg);

private:
	/** Binding Widgets */
	UPROPERTY(meta=(BindWidget))
	USdWidgetPrintMsg* MsgLogWidget;
	
	UPROPERTY(meta=(BindWidget))
	USdWidgetCharacterSelectionPanel* CharacterSelectionPanel;
	/********************/
	
	FDelegateHandle ClientRecvDelegate;

	void BindClientRcv();
};
