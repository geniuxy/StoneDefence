// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameHud/Partners/SdWidgetPartnerList.h"

#include "CommonListView.h"
#include "Characters/Hero/SdCharacterHeroBase.h"
#include "Frameworks/SdAssetManager.h"
#include "Tags/CharacterTags.h"
#include "Widgets/GameHud/Partners/SdButtonPartnerInfo.h"

void USdWidgetPartnerList::AddCharacter(FGameplayTag InCharacterTag, bool bSelected)
{
	UPA_CharacterDefinition* CharacterDef =
		USdAssetManager::Get().GetCharacterDefinition(InCharacterTag.GetTagName());
	if (!CharacterDef) return;

	UPartnerInfo* PartnerInfoData = NewObject<UPartnerInfo>();
	PartnerInfoData->SetCharacterTag(InCharacterTag);
	PartnerInfoData->SetCharacterName(CharacterDef->GetCharacterDisplayName());
	PartnerInfoData->SetCharacterIcon(CharacterDef->GetCharacterIcon());
	CharacterList->AddItem(PartnerInfoData);

	// 如果List只有一个Item，则选中该Item
	if (bSelected)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&]()
			{
				CharacterList->SetSelectedItem(PartnerInfoData);
			})
		);
	}
}

void USdWidgetPartnerList::NativePreConstruct()
{
	Super::NativePreConstruct();

	CharacterList->SetScrollbarVisibility(ESlateVisibility::Collapsed);
}

void USdWidgetPartnerList::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterList->ClearListItems();

	// Test
	AddCharacter(CharacterTags::Sd_Character_Tubaki);
	AddCharacter(CharacterTags::Sd_Character_Cleric);
	
	CharacterList->OnItemSelectionChanged().AddUObject(this, &ThisClass::CharacterSelected);
}

void USdWidgetPartnerList::NativeDestruct()
{
	Super::NativeDestruct();
}

void USdWidgetPartnerList::CharacterSelected(UObject* SelectedUObject)
{
	
}
