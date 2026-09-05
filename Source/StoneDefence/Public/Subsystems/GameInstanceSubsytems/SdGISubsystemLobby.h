// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StoneDefence/StoneDefence.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Widgets/Lobby/FaceSculpt/SdListEntryFaceSculptingFigure.h"
#include "SdGISubsystemLobby.generated.h"

struct FFaceSculptFigureTypeInfo;
class UPA_CharacterDefinition;
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
	DATA_ACCESSOR(int32, CachedSlotIndex)
	DATA_ACCESSOR(UPA_CharacterDefinition*, CurSelectedCharacterDefinition)

private:
	int32 CurSelectedSlotIndex = INDEX_NONE;
	int32 CachedSlotIndex = INDEX_NONE; // 支持缓存上一次槽位的捏脸数据

	UPROPERTY()
	UPA_CharacterDefinition* CurSelectedCharacterDefinition;

	/**********************************************************************/
	/*                         Face Sculpt Figure                         */
	/**********************************************************************/

public:
	TArray<FFaceSculptFigureTypeInfo>& GetCachedFigureSettings() { return CachedFigureSettings; }
	void UpdateCachedFigureSettings(ESdFigureType InType, int32 InValue);

private:
	TArray<FFaceSculptFigureTypeInfo> CachedFigureSettings;
};
