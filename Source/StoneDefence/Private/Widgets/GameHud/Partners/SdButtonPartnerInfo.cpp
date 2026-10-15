// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameHud/Partners/SdButtonPartnerInfo.h"

#include "CommonLazyImage.h"
#include "CommonTextBlock.h"

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
