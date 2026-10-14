// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdTypes/SdMacros.h"
#if SD_ROBOT_TEST
#include "Robots/SdRobotBase.h"
#endif
#include "Widgets/Cores/SdWidgetProtocolReceiver.h"
#include "SdWidgetGameHudMain.generated.h"

class USdWidgetPartnerList;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetGameHudMain : public USdWidgetProtocolReceiver
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel) override;

	virtual void HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg) override;

private:
	UPROPERTY(meta=(BindWidget))
	USdWidgetPartnerList* PartnerList;

	/**********************************************************************/
	/*                            Test Robot                              */
	/**********************************************************************/
protected:
#if SD_ROBOT_TEST
	SdRobotBase Robot;
#endif
};
