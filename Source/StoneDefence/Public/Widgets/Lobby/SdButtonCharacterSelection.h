// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StoneDefence/StoneDefence.h"
#include "Widgets/Components/Button/SdCommonButtonSelection.h"
#include "SdButtonCharacterSelection.generated.h"

class UPA_CharacterDefinition;
class UVerticalBox;

UCLASS()
class STONEDEFENCE_API UCharacterSelectionData : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Id;

	UPROPERTY()
	UPA_CharacterDefinition* CharacterDefinition;

public:
	void SetSelectionId(int32 InId) { Id = InId; }

	DATA_ACCESSOR(UPA_CharacterDefinition*, CharacterDefinition)
};

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdButtonCharacterSelection : public USdCommonButtonSelection
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void NativeConstruct() override;
	virtual void NativeOnClicked() override;

private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonLazyImage* Icon_CreateNew;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* CharacterSlotBox;

	UPROPERTY(meta = (BindWidget))
	UCommonLazyImage* CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CharacterName;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* DateText;
	/********************/

	UPROPERTY()
	UCharacterSelectionData* CharacterSelectionData;
};
