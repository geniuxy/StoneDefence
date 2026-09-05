// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/FaceSculpt/SdWidgetFaceSculptingPageFigure.h"

#include "CommonListView.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"
#include "Widgets/Lobby/FaceSculpt/SdListEntryFaceSculptingFigure.h"

void USdWidgetFaceSculptingPageFigure::ConfigurePageFigure()
{
	if (!USdGISubsystemLobby::Get(this)) return;

	USdGISubsystemLobby* LobbySubsystem = USdGISubsystemLobby::Get(this);
	UPA_CharacterDefinition* SelectedCharacterDefinition = LobbySubsystem->GetCurSelectedCharacterDefinition();
	if (!SelectedCharacterDefinition) return;

	if (LobbySubsystem->GetCachedSlotIndex() != LobbySubsystem->GetCurSelectedSlotIndex())
	{
		FigureOptionListView->ClearListItems();
		LobbySubsystem->GetCachedFigureSettings().Empty();
		for (FFaceSculptFigureTypeInfo DefaultFigureSetting : SelectedCharacterDefinition->GetDefaultFigureSettings())
		{
			UFaceSculptingFigureData* NewFaceSculptingFigureData = NewObject<UFaceSculptingFigureData>();
			NewFaceSculptingFigureData->SetType(DefaultFigureSetting.Type);
			NewFaceSculptingFigureData->SetDefaultValue(DefaultFigureSetting.DefaultValue);
			NewFaceSculptingFigureData->SetMaxValue(DefaultFigureSetting.MaxValue);
			NewFaceSculptingFigureData->SetCurValue(DefaultFigureSetting.DefaultValue * DefaultFigureSetting.MaxValue);\
			FigureOptionListView->AddItem(NewFaceSculptingFigureData);
		}
		LobbySubsystem->SetCachedSlotIndex(LobbySubsystem->GetCurSelectedSlotIndex());
	}
}

void USdWidgetFaceSculptingPageFigure::NativeConstruct()
{
	Super::NativeConstruct();
}

void USdWidgetFaceSculptingPageFigure::NativeOnActivated()
{
	Super::NativeOnActivated();

	ConfigurePageFigure();
}
