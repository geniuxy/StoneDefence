// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameHud/Partners/SdButtonPartnerInfo.h"

#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "Net/UnrealNetwork.h"

void UPartnerInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPartnerInfo, CharacterTag);
	DOREPLIFETIME(UPartnerInfo, SlotIndex);
	DOREPLIFETIME(UPartnerInfo, CharacterName);
	DOREPLIFETIME(UPartnerInfo, CharacterIcon);
}

void USdButtonPartnerInfo::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	PartnerInfoData = Cast<UPartnerInfo>(ListItemObject);
	if (PartnerInfoData)
	{
		CharacterIcon->SetBrushFromTexture(PartnerInfoData->GetCharacterIcon());
		CharacterName->SetText(FText::FromString(PartnerInfoData->GetCharacterName()));
	}
}
