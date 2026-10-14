// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GeniuxyCommonMacro.h"
#include "Widgets/Components/Button/SdCommonButtonSelection.h"
#include "Widgets/Components/ListEntryDataObjects/SdListEntryDataObjectBase.h"
#include "SdButtonPartnerInfo.generated.h"

class ASdCharacterHeroBase;

UCLASS()
class STONEDEFENCE_API UPartnerInfo : public USdListEntryDataObjectBase
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag CharacterTag;

	UPROPERTY()
	FString CharacterName;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> CharacterIcon;

public:
	DATA_ACCESSOR(FGameplayTag, CharacterTag)
	DATA_ACCESSOR(FString, CharacterName)
	DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, CharacterIcon)
};

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdButtonPartnerInfo : public USdCommonButtonSelection
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY()
	UPartnerInfo* PartnerInfoData;
};
