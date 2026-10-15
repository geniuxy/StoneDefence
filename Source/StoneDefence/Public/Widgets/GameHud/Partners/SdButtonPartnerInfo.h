// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/GeniuxyCommonMacro.h"
#include "Widgets/Components/Button/GeCommonButtonSelection.h"
#include "Widgets/Components/ListEntryDataObjects/GeListEntryDataObjectBase.h"
#include "SdButtonPartnerInfo.generated.h"

class ASdCharacterHeroBase;

UCLASS()
class STONEDEFENCE_API UPartnerInfo : public UGeListEntryDataObjectBase
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag CharacterTag;

	UPROPERTY()
	int SlotIndex;

	UPROPERTY()
	FString CharacterName;

	UPROPERTY()
	UTexture2D* CharacterIcon;

public:
	DATA_ACCESSOR(FGameplayTag, CharacterTag)
	DATA_ACCESSOR(int, SlotIndex)
	DATA_ACCESSOR(FString, CharacterName)
	DATA_ACCESSOR(UTexture2D*, CharacterIcon)
};

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdButtonPartnerInfo : public UGeCommonButtonSelection
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonLazyImage* CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CharacterName;
	/********************/

	UPROPERTY()
	UPartnerInfo* PartnerInfoData;
};
