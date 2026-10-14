// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameHud/Partners/SdButtonPartnerInfo.h"

void USdButtonPartnerInfo::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	PartnerInfoData = Cast<UPartnerInfo>(ListItemObject);
}
