// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdCommonButtonImage.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SdCommonButtonSelection.generated.h"

class UCommonListView;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdCommonButtonSelection : public USdCommonButtonImage, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	//~Begin IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnEntryReleased() override; // 只用于EntryReleased（滚动列表等销毁动作）后，将设置项置为Unhovered状态
	//~End IUserObjectListEntry Interface

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	TSoftObjectPtr<UTexture2D> SelectedSoftButtonImage;

	UPROPERTY()
	UCommonListView* OwnerListView;
};
