// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/GeCommonUserWidgetBase.h"
#include "SdWidgetPrintMsg.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetPrintMsg : public UGeCommonUserWidgetBase
{
	GENERATED_BODY()

public:
	void PlayShowMsgAnim();
	void SetLogText(const FText& InMsg);

private:
	/** Binding Widgets */
	UPROPERTY(meta=(BindWidget))
	UCommonTextBlock* MsgLog;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ShowMsg;
	/********************/
};
