// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleNetChannelType.h"
#include "StoneDefenceNetCommonType.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetLobbyMain.generated.h"

class USdWidgetPreviewInputCapture;
class USdCommonButtonImage;
class USdWidgetCreateCharacterPanel;
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

private:
	void HandleCharacterAppearanceResponses(FSimpleChannel* Channel);
	void HandleCheckCharacterNameResponses(FSimpleChannel* Channel);
	void HandleCreateCharacterResponses(FSimpleChannel* Channel);

public:
	void PrintLog(const FString& InMsg);
	void PrintLog(const FText& InMsg);
	void PrintLogByCheckName(ECheckNameType InCheckNameType);

	void BackToCharacterSelectionPanel();
	void SelectRecentCharacter();
	void HandleSelectCharacterSlot(bool bCreateCharacter, int32 Index = INDEX_NONE);

	void ConfigurePreviewInputCapture(AActor* InDisplayActor);

	void CheckNewName(FString NewCharacterName);
	void CreateCharacter(const FSdCharacterAppearance& InCA);

private:
	/** Binding Widgets */
	UPROPERTY(meta=(BindWidget))
	USdWidgetPrintMsg* MsgLogWidget;
	
	UPROPERTY(meta=(BindWidget))
	USdWidgetCharacterSelectionPanel* CharacterSelectionPanel;

	UPROPERTY(meta=(BindWidget))
	USdWidgetCreateCharacterPanel* CreateCharacterPanel;

	UPROPERTY(meta=(BindWidget))
	USdCommonButtonImage* Button_BeginGame;
	
	UPROPERTY(meta=(BindWidget))
	USdWidgetPreviewInputCapture* PreviewInputCaptureWidget;
	/********************/
	
	FDelegateHandle ClientRecvDelegate;

	void BindClientRcv();
	void HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg);
	
	void BeginGame();
};
