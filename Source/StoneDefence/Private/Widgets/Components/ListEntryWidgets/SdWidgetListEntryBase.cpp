// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/ListEntryWidgets/SdWidgetListEntryBase.h"

#include "CommonListView.h"
#include "Widgets/Components/ListEntryDataObjects/SdListEntryDataObjectBase.h"

void USdWidgetListEntryBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	OwnerListView = Cast<UCommonListView>(GetOwningListView());
	if (USdListEntryDataObjectBase* EntryDataObject = Cast<USdListEntryDataObjectBase>(ListItemObject))
	{
		EntryDataObject->OnDataChanged.AddUObject(this, &ThisClass::OnDataChanged);
	}
}
