// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/SdWidgetCharacterSelectionPanel.h"

#include "CommonListView.h"
#include "CommonVisibilitySwitcher.h"
#include "Actors/Lobby/SdActorLobbyDisplay.h"
#include "Engine/StreamableManager.h"
#include "Frameworks/SdAssetManager.h"
#include "Frameworks/PlayerStates/SdPlayerStateLobby.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/DeveloperSettings/SdDataDeveloperSetting.h"
#include "Widgets/Lobby/SdButtonCharacterSelection.h"
#include "Widgets/Lobby/SdWidgetFaceSculpting.h"
#include "Widgets/Lobby/SdWidgetLobbyMain.h"

void USdWidgetCharacterSelectionPanel::NativeConstruct()
{
	Super::NativeConstruct();

	USdAssetManager::Get().LoadCharacterDefinitions(
		FStreamableDelegate::CreateUObject(this, &ThisClass::InitSelectionListView)
	);

	// OnItemSelectionChanged()在取消选择时，不会做任何反应
	SelectionListView->OnItemSelectionChanged().AddUObject(this, &ThisClass::CharacterSelected);

	SpawnCharacterDisplay();
}

void USdWidgetCharacterSelectionPanel::NativePreConstruct()
{
	Super::NativePreConstruct();

	SelectionListView->SetScrollbarVisibility(ESlateVisibility::Collapsed);
}

void USdWidgetCharacterSelectionPanel::InitSelectionListView()
{
	UPA_CharacterDefinition* TubakiDef =
		USdAssetManager::Get().GetCharacterDefinition(FName("PA_CharacterDefinition_Tubaki"));
	if (!TubakiDef) return;

	if (const USdDataDeveloperSetting* DataDeveloperSettings = GetDefault<USdDataDeveloperSetting>())
	{
		for (int i = 0; i < DataDeveloperSettings->MaxCharacterSelectionNum; ++i)
		{
			UCharacterSelectionData* CharacterSelectionData = NewObject<UCharacterSelectionData>();
			CharacterSelectionData->SetSlotIndex(i);
			CharacterSelectionData->SetCharacterDefinition(TubakiDef);
			CharacterSelectionData->SetSlotIsEmpty(true);
			SelectionListView->AddItem(CharacterSelectionData);
		}
	}
	
	if (bPendingUpdateAppearances)
	{
		UpdateCharacterAppearances();
	}
	bInitCharacterDefinitions = true;
}

void USdWidgetCharacterSelectionPanel::CharacterSelected(UObject* SelectedUObject)
{
	ASdPlayerStateLobby* PlayerState = GetOwningPlayerState<ASdPlayerStateLobby>();
	if (!PlayerState) return;

	if (const UCharacterSelectionData* CharacterSelectionData = Cast<UCharacterSelectionData>(SelectedUObject))
	{
		PlayerState->SetSelectedCharacterDefinition(CharacterSelectionData->GetCharacterDefinition());
		if (ActorLobbyDisplay)
		{
			ActorLobbyDisplay->ConfigureWithCharacterDefinition(CharacterSelectionData->GetCharacterDefinition());
		}

		if (CharacterSelectionData->IsSlotEmpty())
		{
			Switcher->SetActiveWidget(FaceSculptingWidget);
			if (USdWidgetLobbyMain* LobbyMain = GetParentWidget<USdWidgetLobbyMain>())
			{
				LobbyMain->HandleSelectCharacterSlot(true);
			}
		}
		else
		{
			Switcher->SetActiveWidget(SelectionListView);
			if (USdWidgetLobbyMain* LobbyMain = GetParentWidget<USdWidgetLobbyMain>())
			{
				LobbyMain->HandleSelectCharacterSlot(false);
			}
		}
	}
}

void USdWidgetCharacterSelectionPanel::SpawnCharacterDisplay()
{
	if (ActorLobbyDisplay) return;
	if (!ActorLobbyDisplayClass) return;

	FTransform CharacterDisplayTransform = FTransform::Identity;
	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
	{
		CharacterDisplayTransform = PlayerStart->GetActorTransform();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ActorLobbyDisplay =
		GetWorld()->SpawnActor<ASdActorLobbyDisplay>(ActorLobbyDisplayClass, CharacterDisplayTransform, SpawnParams);
	GetOwningPlayer()->SetViewTarget(ActorLobbyDisplay);
}

void USdWidgetCharacterSelectionPanel::BackToCharacterSelectionPanel()
{
	Switcher->SetActiveWidget(SelectionListView);
	SelectionListView->ClearSelection();
	if (ActorLobbyDisplay)
	{
		ActorLobbyDisplay->ClearCharacterDefinition();
	}
}

void USdWidgetCharacterSelectionPanel::UpdateCharacterAppearances()
{
	ASdPlayerStateLobby* PlayerState = GetOwningPlayerState<ASdPlayerStateLobby>();
	if (!PlayerState) return;

	if (!bInitCharacterDefinitions)
	{
		bPendingUpdateAppearances = true;
		return;
	}
	bPendingUpdateAppearances = false;

	for (UObject* Item : SelectionListView->GetListItems())
	{
		UCharacterSelectionData* CharacterSelectionData = Cast<UCharacterSelectionData>(Item);
		if (!CharacterSelectionData) continue;

		FSdCharacterAppearance* CharacterAppearance = PlayerState->GetCachedCharacterAppearances().FindByPredicate(
			[&](FSdCharacterAppearance InCharacterAppearance)
			{
				return InCharacterAppearance.SlotIndex == CharacterSelectionData->GetSlotIndex();
			}
		);

		if (CharacterAppearance)
		{
			CharacterSelectionData->SetSlotIsEmpty(CharacterAppearance->IsEmpty());
			CharacterSelectionData->SetCharacterName(CharacterAppearance->Name);
			CharacterSelectionData->SetLastLoginTimeStr(CharacterAppearance->LastLoginTime);
			UPA_CharacterDefinition* DisplayCharacterDefinition =
				USdAssetManager::Get().GetCharacterDefinition(FName(CharacterAppearance->DisplayAssetName));
			if (DisplayCharacterDefinition)
			{
				CharacterSelectionData->SetCharacterDefinition(DisplayCharacterDefinition);
			}

			CharacterSelectionData->OnDataChanged.Broadcast();
		}
	}
}

void USdWidgetCharacterSelectionPanel::SelectRecentCharacter()
{
	FDateTime RecentlyDate;
	int32 RecentlyIndex = INDEX_NONE;
	for (UObject* Item : SelectionListView->GetListItems())
	{
		UCharacterSelectionData* CharacterSelectionData = Cast<UCharacterSelectionData>(Item);
		if (!CharacterSelectionData) continue;

		FDateTime CurDate;
		FDateTime::Parse(CharacterSelectionData->GetLastLoginTimeStr(), CurDate);
		if (CurDate > RecentlyDate)
		{
			RecentlyDate = CurDate;
			RecentlyIndex = SelectionListView->GetIndexForItem(Item);
		}
	}

	if (RecentlyIndex != INDEX_NONE)
	{
		SelectionListView->SetSelectedIndex(RecentlyIndex);
	}
}
