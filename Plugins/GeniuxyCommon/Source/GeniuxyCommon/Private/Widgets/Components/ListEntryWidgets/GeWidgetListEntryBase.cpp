// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/ListEntryWidgets/GeWidgetListEntryBase.h"

#include "CommonListView.h"
#include "Widgets/Components/ListEntryDataObjects/GeListEntryDataObjectBase.h"

void UGeWidgetListEntryBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	OwnerListView = Cast<UCommonListView>(GetOwningListView());
	if (UGeListEntryDataObjectBase* EntryDataObject = Cast<UGeListEntryDataObjectBase>(ListItemObject))
	{
		EntryDataObject->OnDataChanged.AddUObject(this, &ThisClass::OnDataChanged);
	}
}
