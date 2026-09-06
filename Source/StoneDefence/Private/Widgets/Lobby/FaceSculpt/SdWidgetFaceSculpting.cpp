// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/FaceSculpt/SdWidgetFaceSculpting.h"

#include "CommonListView.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "Actors/SdActorPreview.h"
#include "Settings/DeveloperSettings/SdDataDeveloperSetting.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"
#include "Widgets/Lobby/SdWidgetCharacterSelectionPanel.h"
#include "Widgets/Lobby/FaceSculpt/SdButtonFaceSculptType.h"

void USdWidgetFaceSculpting::NativeConstruct()
{
	Super::NativeConstruct();

	PageListView->ClearListItems();
	if (const USdDataDeveloperSetting* DataDeveloperSettings = GetDefault<USdDataDeveloperSetting>())
	{
		for (FFaceSculptPageDataInfo FaceSculptPageDataInfo : DataDeveloperSettings->FaceSculptPageDataInfoList)
		{
			UFaceSculptPageData* FaceSculptPageData = NewObject<UFaceSculptPageData>();
			FaceSculptPageData->SetInfo(FaceSculptPageDataInfo);
			PageListView->AddItem(FaceSculptPageData);
		}
	}
	PageListView->OnItemSelectionChanged().AddUObject(this, &ThisClass::PageSelected);

	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
		{
			PageListView->SetSelectedIndex(0);
		})
	);
}

void USdWidgetFaceSculpting::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void USdWidgetFaceSculpting::PageSelected(UObject* InSelectedObject)
{
	if (const UFaceSculptPageData* FaceSculptPageData = Cast<UFaceSculptPageData>(InSelectedObject))
	{
		int PageIndex = static_cast<int>(FaceSculptPageData->GetType());
		Switcher->SetActiveWidgetIndex(PageIndex);
		if (USdGISubsystemLobby::Get(this)->GetActorLobbyPreview())
		{
			USdGISubsystemLobby::Get(this)->GetActorLobbyPreview()->SetIsModifying(
				FaceSculptPageData->GetType() < ESdFaceSculptPageType::FSP_TALENT
			);
		}
	}
}
