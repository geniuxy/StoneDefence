// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/SdWidgetCharacterSelectionPanel.h"

#include "CommonListView.h"
#include "CommonTextBlock.h"
#include "CommonVisibilitySwitcher.h"
#include "Actors/SdActorPreview.h"
#include "Components/VerticalBox.h"
#include "Engine/StreamableManager.h"
#include "Frameworks/SdAssetManager.h"
#include "Frameworks/GameInstance/SdGameInstance.h"
#include "Frameworks/PlayerStates/SdPlayerStateLobby.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Protocol/LobbyProtocol.h"
#include "SdTypes/SdMacros.h"
#include "Settings/DeveloperSettings/SdDataDeveloperSetting.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"
#include "Widgets/Lobby/SdButtonCharacterSelection.h"
#include "Widgets/Lobby/SdWidgetLobbyMain.h"
#include "Widgets/Lobby/FaceSculpt/SdWidgetFaceSculpting.h"

void USdWidgetCharacterSelectionPanel::NativeConstruct()
{
	Super::NativeConstruct();

	FaceSculptingWidget->SetParentWidget(this);

	USdAssetManager::Get().LoadCharacterDefinitions(
		FStreamableDelegate::CreateUObject(this, &ThisClass::InitSelectionListView)
	);

	// OnItemSelectionChanged()在取消选择时，不会做任何反应
	SelectionListView->OnItemSelectionChanged().AddUObject(this, &ThisClass::CharacterSelected);

	Button_Edit->OnReleased().AddUObject(this, &ThisClass::HandleEditCharacter);
	Button_Delete->OnReleased().AddUObject(this, &ThisClass::HandleDeleteCharacter);

	SpawnCharacterPreview();

	// 在收到服务端信息之前都是不可点击的
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USdWidgetCharacterSelectionPanel::NativePreConstruct()
{
	Super::NativePreConstruct();

	SelectionListView->SetScrollbarVisibility(ESlateVisibility::Collapsed);
}

void USdWidgetCharacterSelectionPanel::OnSetParentWidget()
{
	if (USdWidgetLobbyMain* LobbyMain = GetParentWidget<USdWidgetLobbyMain>())
	{
		LobbyMain->ConfigurePreviewInputCapture(USdGISubsystemLobby::Get(this)->GetActorLobbyPreview());
	}
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
	USdGISubsystemLobby* LobbySubsystem = USdGISubsystemLobby::Get(this);
	if (!LobbySubsystem) return;

	if (const UCharacterSelectionData* CharacterSelectionData = Cast<UCharacterSelectionData>(SelectedUObject))
	{
		LobbySubsystem->SetCurSelectedCharacterDefinition(CharacterSelectionData->GetCharacterDefinition());
		LobbySubsystem->SetCurSelectedSlotIndex(CharacterSelectionData->GetSlotIndex());
		
		if (ASdActorPreview* ActorLobbyPreview = LobbySubsystem->GetActorLobbyPreview())
		{
			ActorLobbyPreview->ConfigureWithCharacterDefinition(CharacterSelectionData->GetCharacterDefinition());

			// 根据服务器上的身材数据赋值给ActorLobbyPreview
			FSdCharacterAppearance* CharacterAppearance = PlayerState->GetCachedCharacterAppearances().FindByPredicate(
				[&](const FSdCharacterAppearance& InCharacterAppearance)
				{
					return InCharacterAppearance.SlotIndex == CharacterSelectionData->GetSlotIndex();
				}
			);
			if (CharacterAppearance)
			{
				ActorLobbyPreview->UpdateFigureTypeSize(CharacterAppearance->FigureSizeStr);
			}
		}

		if (CharacterSelectionData->IsSlotEmpty())
		{
			Switcher->SetActiveWidget(FaceSculptingWidget);
			PanelTitle->SetText(FText::FromString(TEXT("角色创建")));
		}
		else
		{
			Switcher->SetActiveWidget(SelectionBox);
			PanelTitle->SetText(FText::FromString(TEXT("角色选择")));
		}

		if (USdWidgetLobbyMain* LobbyMain = GetParentWidget<USdWidgetLobbyMain>())
		{
			LobbyMain->HandleSelectCharacterSlot(CharacterSelectionData->IsSlotEmpty());
		}
	}
}

void USdWidgetCharacterSelectionPanel::HandleEditCharacter()
{
	USdGISubsystemLobby* LobbySubsystem = USdGISubsystemLobby::Get(this);
	if (!LobbySubsystem) return;
	// SEND_DATA(SP_DeleteCharacterRequests,  ClientGameInstance->GetUserData().Id, NewCharacterName);
}

void USdWidgetCharacterSelectionPanel::HandleDeleteCharacter()
{
	USdGISubsystemLobby* LobbySubsystem = USdGISubsystemLobby::Get(this);
	if (!LobbySubsystem) return;
	int32 CurSelectedSlotIndex = LobbySubsystem->GetCurSelectedSlotIndex();
	const USdDataDeveloperSetting* DataDeveloperSettings = GetDefault<USdDataDeveloperSetting>();
	if (!DataDeveloperSettings) return;
	if (CurSelectedSlotIndex >= 0 && CurSelectedSlotIndex < DataDeveloperSettings->MaxCharacterSelectionNum)
	{
		SEND_DATA(SP_DeleteCharacterRequests,  ClientGameInstance->GetUserData().Id, CurSelectedSlotIndex);
	}
}

void USdWidgetCharacterSelectionPanel::SpawnCharacterPreview()
{
	if (!USdGISubsystemLobby::Get(this)) return;
	if (USdGISubsystemLobby::Get(this)->GetActorLobbyPreview()) return;
	if (!ActorLobbyPreviewClass) return;

	FTransform CharacterDisplayTransform = FTransform::Identity;
	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
	{
		CharacterDisplayTransform = PlayerStart->GetActorTransform();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASdActorPreview* ActorLobbyPreview =
		GetWorld()->SpawnActor<ASdActorPreview>(ActorLobbyPreviewClass, CharacterDisplayTransform, SpawnParams);
	if (ActorLobbyPreview)
	{
		USdGISubsystemLobby::Get(this)->SetActorLobbyPreview(ActorLobbyPreview);
		GetOwningPlayer()->SetViewTarget(ActorLobbyPreview);
	}
}

void USdWidgetCharacterSelectionPanel::BackToCharacterSelectionPanel()
{
	Switcher->SetActiveWidget(SelectionBox);
	PanelTitle->SetText(FText::FromString(TEXT("角色选择")));
	SelectionListView->ClearSelection();
	if (USdGISubsystemLobby::Get(this)->GetActorLobbyPreview())
	{
		USdGISubsystemLobby::Get(this)->GetActorLobbyPreview()->ClearCharacterDefinition();
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
		}
		else
		{
			CharacterSelectionData->SetSlotIsEmpty(true);
			CharacterSelectionData->SetCharacterName("");
			CharacterSelectionData->SetLastLoginTimeStr("");
			if (UPA_CharacterDefinition* TubakiDef =
				USdAssetManager::Get().GetCharacterDefinition(FName("PA_CharacterDefinition_Tubaki")))
			{
				CharacterSelectionData->SetCharacterDefinition(TubakiDef);
			}
		}
		CharacterSelectionData->OnDataChanged.Broadcast();
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
