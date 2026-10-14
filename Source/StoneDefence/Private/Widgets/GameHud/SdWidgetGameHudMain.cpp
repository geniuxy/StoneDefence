// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameHud/SdWidgetGameHudMain.h"

#include "Characters/Hero/SdCharacterHeroBase.h"
#include "Widgets/GameHud/Partners/SdWidgetPartnerList.h"

#define LOCTEXT_NAMESPACE "USdWidgetGameHudMain"

void USdWidgetGameHudMain::NativeConstruct()
{
	Super::NativeConstruct();

#if SD_ROBOT_TEST
	if (ASdCharacterHeroBase* InPlayer = GetOwningPlayerPawn<ASdCharacterHeroBase>())
	{
		Robot.GetGateRobot().StartDelegate.BindUObject(InPlayer, &ASdCharacterHeroBase::CallServerUpdateFaceSculpting);
	}
	
	Robot.InitGate("127.0.0.1", 11222);
#endif
}

void USdWidgetGameHudMain::NativeDestruct()
{
	Super::NativeDestruct();
}

void USdWidgetGameHudMain::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

#if SD_ROBOT_TEST
	Robot.Tick(InDeltaTime);
#endif
}

void USdWidgetGameHudMain::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch (ProtocolNumber)
	{
	case 1:
		break;
	default:
		break;
	}
}

void USdWidgetGameHudMain::HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg)
{
	if (InType == HAND_SHAKE_SUCCESS)
	{
		
	}
}

#undef LOCTEXT_NAMESPACE
