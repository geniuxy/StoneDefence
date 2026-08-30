// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Common/SdWidgetPrintMsg.h"

#include "CommonTextBlock.h"

void USdWidgetPrintMsg::PlayShowMsgAnim()
{
	PlayAnimation(ShowMsg);
}

void USdWidgetPrintMsg::SetLogText(const FText& InMsg)
{
	MsgLog->SetText(InMsg);
}
