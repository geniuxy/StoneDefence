// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SdListEntryDataObjectBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSelectionDataChanged);
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdListEntryDataObjectBase : public UObject
{
	GENERATED_BODY()

public:
	FOnSelectionDataChanged OnDataChanged;
};
