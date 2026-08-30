// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/SdButtonCharacterSelection.h"

#include "CommonListView.h"

void USdButtonCharacterSelection::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	CharacterSelectionData = Cast<UCharacterSelectionData>(ListItemObject);
}

void USdButtonCharacterSelection::NativeOnClicked()
{
	Super::NativeOnClicked();

	if (IsValid(OwnerListView) && IsValid(CharacterSelectionData))
	{
		OwnerListView->SetSelectedItem(CharacterSelectionData);
	}
}
