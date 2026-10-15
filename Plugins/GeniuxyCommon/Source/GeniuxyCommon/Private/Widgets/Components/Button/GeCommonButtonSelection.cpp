// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/Button/GeCommonButtonSelection.h"

#include "CommonListView.h"
#include "Widgets/Components/ListEntryDataObjects/GeListEntryDataObjectBase.h"

void UGeCommonButtonSelection::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	OwnerListView = Cast<UCommonListView>(GetOwningListView());
	if (UGeListEntryDataObjectBase* EntryDataObject = Cast<UGeListEntryDataObjectBase>(ListItemObject))
	{
		EntryDataObject->OnDataChanged.AddUObject(this, &ThisClass::OnDataChanged);
	}
}

void UGeCommonButtonSelection::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);
	
	if (!SelectedSoftButtonImage.ToSoftObjectPath().IsValid()) return;

	if (bIsSelected)
	{
		SetButtonDisplayImage(SelectedSoftButtonImage);
	}
	else
	{
		if (SoftButtonImage.ToSoftObjectPath().IsValid())
		{
			SetButtonDisplayImage(SoftButtonImage);
		}
	}
}

void UGeCommonButtonSelection::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();

	if (SelectedSoftButtonImage.ToSoftObjectPath().IsValid() && SoftButtonImage.ToSoftObjectPath().IsValid())
	{
		SetButtonDisplayImage(SoftButtonImage);
	}
}
