// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/Button/SdCommonButtonSelection.h"

#include "CommonListView.h"

void USdCommonButtonSelection::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	OwnerListView = Cast<UCommonListView>(GetOwningListView());
}

void USdCommonButtonSelection::NativeOnItemSelectionChanged(bool bIsSelected)
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

void USdCommonButtonSelection::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();
	
	if (SelectedSoftButtonImage.ToSoftObjectPath().IsValid() && SoftButtonImage.ToSoftObjectPath().IsValid())
	{
		SetButtonDisplayImage(SoftButtonImage);
	}
}
