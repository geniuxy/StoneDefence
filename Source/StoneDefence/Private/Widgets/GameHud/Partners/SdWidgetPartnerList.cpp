// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameHud/Partners/SdWidgetPartnerList.h"

#include "CommonListView.h"
#include "Characters/Hero/SdCharacterHeroBase.h"
#include "Frameworks/SdAssetManager.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemCharacter.h"
#include "Tags/CharacterTags.h"
#include "Widgets/GameHud/Partners/SdButtonPartnerInfo.h"

void USdWidgetPartnerList::AddCharacter(FGameplayTag InCharacterTag, bool bSelected)
{
	UPA_CharacterDefinition* CharacterDef =
		USdAssetManager::Get().GetCharacterDefinition(InCharacterTag.GetTagName());
	if (!CharacterDef)
	{
		USdGISubsystemCharacter* Subsystem = USdGISubsystemCharacter::Get(this);
		if (!Subsystem) return;

		Subsystem->LoadCharacter(
			InCharacterTag,
			FOnCharacterDefinitionLoaded::CreateWeakLambda(
				this,
				// 必须按值捕获：InCharacterTag / bSelected 是函数参数，
				// 这个 lambda 是在异步加载完成后才执行的，那时栈帧早已销毁，
				// 用 [&] 会读到悬空引用（tag 常常变成 NAME_None）。
				[this, InCharacterTag, bSelected](const UPA_CharacterDefinition* Definition)
				{
					if (!Definition) return;
					AddCharacter(InCharacterTag, bSelected);
				}
			)
		);
		return;
	}

	UPartnerInfo* PartnerInfoData = NewObject<UPartnerInfo>();
	PartnerInfoData->SetCharacterTag(InCharacterTag);
	PartnerInfoData->SetCharacterName(CharacterDef->GetCharacterDisplayName());
	PartnerInfoData->SetCharacterIcon(CharacterDef->LoadIcon());
	PartnerInfoData->SetSlotIndex(CharacterList->GetNumItems());
	CharacterList->AddItem(PartnerInfoData);

	// 如果List只有一个Item，则选中该Item
	if (bSelected)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this, PartnerInfoData]()
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
	AddCharacter(CharacterTags::Sd_Character_Cleric, false);
	
	CharacterList->OnItemSelectionChanged().AddUObject(this, &ThisClass::CharacterSelected);
}

void USdWidgetPartnerList::NativeDestruct()
{
	Super::NativeDestruct();
}

void USdWidgetPartnerList::CharacterSelected(UObject* SelectedUObject)
{
	
}
