// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/PlayerControllers/SdPlayerControllerBase.h"
#include "SdPlayerControllerGameBase.generated.h"

class USdWidgetGameHudMain;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API ASdPlayerControllerGameBase : public ASdPlayerControllerBase
{
	GENERATED_BODY()

public:
	ASdPlayerControllerGameBase();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<USdWidgetGameHudMain> GameHudMainClass;
	UPROPERTY()
	USdWidgetGameHudMain* GameHudMain = nullptr;
};
