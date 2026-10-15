// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/Cores/GeCommonUserWidgetBase.h"
#include "GeWidgetListEntryBase.generated.h"

class UCommonListView;
/**
 * 
 */
UCLASS(Blueprintable)
class GENIUXYCOMMON_API UGeWidgetListEntryBase : public UGeCommonUserWidgetBase, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	UPROPERTY()
	UCommonListView* OwnerListView;

	virtual void OnDataChanged()
	{
	}
};
