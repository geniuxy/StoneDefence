// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleNetChannelType.h"
#include "Widgets/Cores/SdWidgetProtocolReceiver.h"
#include "SdWidgetLoginMain.generated.h"

class USdWidgetRegisterInfo;
class USdWidgetPrintMsg;
class USdWidgetLoginInfo;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetLoginMain : public USdWidgetProtocolReceiver
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel) override;

public:
	void SignIn(FString InAccount, FString InPassword);
	void Register();
	void SendRegisterInfo(FString InRegisterInfo);

	void PrintLog(const FString& InMsg);
	void PrintLog(const FText& InMsg);

private:
	/** Binding Widgets */
	UPROPERTY(meta=(BindWidget))
	USdCommonUserWidgetBase* LinkWidget;

	UPROPERTY(meta=(BindWidget))
	USdWidgetLoginInfo* LoginInfo;

	UPROPERTY(meta = (BindWidget))
	USdWidgetRegisterInfo* RegisterInfo;

	UPROPERTY(meta=(BindWidget))
	USdWidgetPrintMsg* MsgLogWidget;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* LoginInfoFadeIn;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* LoginInfoFadeOut;
	/********************/

	void ShowServerLinkingInfo(ESimpleNetErrorType InType, const FString& InMsg);

	void HandleLoginResponses(FSimpleChannel* Channel);
	void HandleRegisterResponses(FSimpleChannel* Channel);
};
