// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Components/ListEntryDataObjects/SdListEntryDataObjectBase.h"
#include "StoneDefence/StoneDefence.h"
#include "Widgets/Components/Button/SdCommonButtonSelection.h"
#include "SdButtonFaceSculptType.generated.h"

UENUM()
enum class ESdFaceSculptPageType : uint8
{
	FSP_FIGURE UMETA(DisplayName = "身材"),
	FSP_FACE UMETA(DisplayName = "容貌"),
	FSP_TALENT UMETA(DisplayName = "天赋"),
	FSP_NUM
};

USTRUCT(BlueprintType)
struct FFaceSculptPageDataInfo // 捏脸界面各个类别按钮的相关信息
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ESdFaceSculptPageType Type;
};

UCLASS()
class STONEDEFENCE_API UFaceSculptPageData : public USdListEntryDataObjectBase
{
	GENERATED_BODY()

	UPROPERTY()
	FFaceSculptPageDataInfo Info;

public:
	ESdFaceSculptPageType GetType() const { return Info.Type; }
	DATA_ACCESSOR(FFaceSculptPageDataInfo, Info)
};

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdButtonFaceSculptType : public USdCommonButtonSelection
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY()
	UFaceSculptPageData* FaceSculptPageData;
};
