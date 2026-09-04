// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StoneDefence/StoneDefence.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SdGISubsystemLobby.generated.h"

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdGISubsystemLobby : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static USdGISubsystemLobby* Get(const UObject* WorldContextObject);

	/**********************************************************************/
	/*                        Create And Selection                        */
	/**********************************************************************/
public:
	DATA_ACCESSOR(int32, CurSelectedSlotIndex)

private:
	int32 CurSelectedSlotIndex = INDEX_NONE;
};
