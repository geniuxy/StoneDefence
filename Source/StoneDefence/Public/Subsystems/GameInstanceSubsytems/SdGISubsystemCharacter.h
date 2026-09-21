// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SdGISubsystemCharacter.generated.h"

class UPA_CharacterDefinition;

DECLARE_DELEGATE_OneParam(FOnCharacterDefinitionLoaded, UPA_CharacterDefinition*);

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdGISubsystemCharacter : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static USdGISubsystemCharacter* Get(const UObject* WorldContextObject);

	void LoadCharacter(const FPrimaryAssetId& InAssetId, FOnCharacterDefinitionLoaded Callback);
};
