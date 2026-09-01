// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdTypes/ListEntryDataObjects/SdListEntryDataObjectBase.h"
#include "StoneDefence/StoneDefence.h"
#include "Widgets/Components/Button/SdCommonButtonSelection.h"
#include "SdButtonCharacterSelection.generated.h"

class USizeBox;
class UCommonVisibilitySwitcher;
class UPA_CharacterDefinition;
class UVerticalBox;

UCLASS()
class STONEDEFENCE_API UCharacterSelectionData : public USdListEntryDataObjectBase
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY()
	FString CharacterName;

	UPROPERTY()
	FString LastLoginTimeStr;

	UPROPERTY()
	UPA_CharacterDefinition* CharacterDefinition;

	UPROPERTY()
	bool bIsEmpty;

public:
	DATA_ACCESSOR(int32, SlotIndex)
	DATA_ACCESSOR(FString, CharacterName)
	DATA_ACCESSOR(FString, LastLoginTimeStr)
	DATA_ACCESSOR(UPA_CharacterDefinition*, CharacterDefinition)
	FORCEINLINE bool IsSlotEmpty() const { return bIsEmpty; }
	void SetSlotIsEmpty(bool bInIsEmpty) { bIsEmpty = bInIsEmpty; }
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

	virtual void OnDataChanged() override;

private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonVisibilitySwitcher* Switcher;

	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox_CreateNew;
	
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

	void UpdateButtonInfo();
};
