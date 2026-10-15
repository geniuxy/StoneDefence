// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GeListEntryDataObjectBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSelectionDataChanged);
/**
 * 
 */
UCLASS()
class GENIUXYCOMMON_API UGeListEntryDataObjectBase : public UObject
{
	GENERATED_BODY()

public:
	FOnSelectionDataChanged OnDataChanged;
};
