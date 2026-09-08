// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/FaceSculpt/SdWidgetFaceSculptingPageFigure.h"

#include "CommonListView.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "Frameworks/PlayerStates/SdPlayerStateLobby.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"
#include "Widgets/Lobby/FaceSculpt/SdListEntryFaceSculptingFigure.h"

void USdWidgetFaceSculptingPageFigure::ConfigurePageFigure()
{
	if (!USdGISubsystemLobby::Get(this)) return;
	ASdPlayerStateLobby* PlayerState = GetOwningPlayerState<ASdPlayerStateLobby>();
	if (!PlayerState) return;

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
			NewFaceSculptingFigureData->SetMinValue(DefaultFigureSetting.MinValue);

			int CurValue = FMath::Max(
				DefaultFigureSetting.MinValue, DefaultFigureSetting.DefaultValue * DefaultFigureSetting.MaxValue
			);
			// 根据服务器的FigureSizeStr值进行修改
			if (PlayerState->GetCurSelectedCharacterAppearance().IsSet() &&
				!PlayerState->GetCurSelectedCharacterAppearance()->IsEmpty())
			{
				FString FigureSizeStr = PlayerState->GetCurSelectedCharacterAppearance().GetValue().FigureSizeStr;
				TArray<FString> StrFigureSizeArray;
				FigureSizeStr.ParseIntoArray(StrFigureSizeArray, TEXT("|"));
				for (const FString& StrFigureSize : StrFigureSizeArray)
				{
					TArray<FString> StrFigureTypeAndSize;
					StrFigureSize.ParseIntoArray(StrFigureTypeAndSize, TEXT(","));
					if (StrFigureTypeAndSize.Num() != 2) continue;

					int TypeIndex = FCString::Atoi(*StrFigureTypeAndSize[0]);
					if (TypeIndex == static_cast<int>(DefaultFigureSetting.Type))
					{
						CurValue = FCString::Atoi(*StrFigureTypeAndSize[1]);
						break;
					}
				}
			}
			NewFaceSculptingFigureData->SetCurValue(CurValue);
			LobbySubsystem->UpdateCachedFigureSettings(DefaultFigureSetting.Type, CurValue);
			
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
