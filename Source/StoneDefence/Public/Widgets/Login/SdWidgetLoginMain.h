// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleNetChannelType.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetLoginMain.generated.h"

class USdWidgetRegisterInfo;
class USdWidgetPrintMsg;
class USdWidgetLoginInfo;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetLoginMain : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel) override;

public:
	void SignIn(FString InAccount, FString InPassword);
	void Register();
	void Register(FString InRegisterInfo);

	void PrintLog(const FString& InMsg);
	void PrintLog(const FText& InMsg);

private:
	/** Binding Widgets */
	UPROPERTY(meta=(BindWidget))
	USdCommonUserWidgetBase* LinkWidget;

	UPROPERTY(meta=(BindWidget))
	USdWidgetLoginInfo* LoginInfo;

	UPROPERTY(meta = (BindWidgetOptional))
	USdWidgetRegisterInfo* RegisterInfo;

	UPROPERTY(meta=(BindWidget))
	USdWidgetPrintMsg* MsgLogWidget;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* LoginInfoFadeIn;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* LoginInfoFadeOut;
	/********************/
	
	FDelegateHandle ClientRecvDelegate;
	void BindClientRcv();

	void ShowServerLinkingInfo(ESimpleNetErrorType InType, const FString& InMsg);

	void HandleLoginResponses(FSimpleChannel* Channel);
	void HandleRegisterResponses(FSimpleChannel* Channel);
};
