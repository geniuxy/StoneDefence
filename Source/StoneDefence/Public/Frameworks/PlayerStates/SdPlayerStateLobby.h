// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdPlayerStateBase.h"
#include "StoneDefenceNetCommonType.h"
#include "SdPlayerStateLobby.generated.h"

class UPA_CharacterDefinition;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API ASdPlayerStateLobby : public ASdPlayerStateBase
{
	GENERATED_BODY()

public:
	void SetSelectedCharacterDefinition(const UPA_CharacterDefinition* NewDefinition);

private:
	FSdCharacterAppearances CachedCharacterAppearances;

public:
	FSdCharacterAppearances& GetCachedCharacterAppearances() { return CachedCharacterAppearances; }
};
