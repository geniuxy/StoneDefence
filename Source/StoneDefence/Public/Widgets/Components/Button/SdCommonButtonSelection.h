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
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// 滚动列表时，也会触发NativeOnItemSelectionChanged和NativeOnEntryReleased
	// SetSelectedItem后，会触发NativeOnItemSelectionChanged
	// 只在EntryReleased（滚动列表等销毁动作）后，NativeOnEntryReleased将设置项置为Unhovered状态
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnEntryReleased() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	TSoftObjectPtr<UTexture2D> SelectedSoftButtonImage;

	UPROPERTY()
	UCommonListView* OwnerListView;
};
