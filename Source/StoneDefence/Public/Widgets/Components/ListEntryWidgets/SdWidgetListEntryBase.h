// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetListEntryBase.generated.h"

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetListEntryBase : public USdCommonUserWidgetBase, public IUserObjectListEntry
{
	GENERATED_BODY()
};
