// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdPlayerControllerBase.h"
#include "SdPlayerControllerLobby.generated.h"

class USdWidgetLobbyMain;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API ASdPlayerControllerLobby : public ASdPlayerControllerBase
{
	GENERATED_BODY()

public:
	ASdPlayerControllerLobby();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<USdWidgetLobbyMain> LobbyMainClass;
	UPROPERTY()
	USdWidgetLobbyMain* LobbyMain = nullptr;
};
