// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Widgets/Lobby/FaceSculpt/SdButtonFaceSculptType.h"
#include "SdDataDeveloperSetting.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Data Settings"))
class STONEDEFENCE_API USdDataDeveloperSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Lobby")
	int32 MaxCharacterSelectionNum;
	
	UPROPERTY(Config, EditAnywhere, Category = "Lobby")
	TArray<FFaceSculptPageDataInfo> FaceSculptPageDataInfoList;
};
