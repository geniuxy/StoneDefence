// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Components/Button/SdCommonButtonSelection.h"
#include "SdButtonCharacterSelection.generated.h"

UCLASS()
class STONEDEFENCE_API UCharacterSelectionData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	int32 Id;
};
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdButtonCharacterSelection : public USdCommonButtonSelection
{
	GENERATED_BODY()

protected:
	//~Begin IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	//~End IUserObjectListEntry Interface

	virtual void NativeOnClicked() override;

private:
	UPROPERTY()
	UCharacterSelectionData* CharacterSelectionData;
};
