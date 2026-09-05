// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/SdButtonCharacterSelection.h"

#include "CommonListView.h"
#include "CommonTextBlock.h"
#include "CommonVisibilitySwitcher.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"

void USdButtonCharacterSelection::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	CharacterSelectionData = Cast<UCharacterSelectionData>(ListItemObject);
	UpdateButtonInfo();
}

void USdButtonCharacterSelection::NativeConstruct()
{
	Super::NativeConstruct();
}

void USdButtonCharacterSelection::NativeOnClicked()
{
	Super::NativeOnClicked();

	if (IsValid(OwnerListView) && IsValid(CharacterSelectionData))
	{
		OwnerListView->SetSelectedItem(CharacterSelectionData);
	}
}

void USdButtonCharacterSelection::OnDataChanged()
{
	UpdateButtonInfo();
}

void USdButtonCharacterSelection::UpdateButtonInfo()
{
	if (!CharacterSelectionData) return;

	if (CharacterSelectionData->IsSlotEmpty())
	{
		Switcher->SetActiveWidget(SizeBox_CreateNew);
	}
	else
	{
		Switcher->SetActiveWidget(CharacterSlotBox);
		CharacterName->SetText(FText::FromString(CharacterSelectionData->GetCharacterName()));
		DateText->SetText(FText::FromString(CharacterSelectionData->GetLastLoginTimeStr()));
	}
}
