// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetCharacterSelectionPanel.generated.h"

class UCommonListView;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetCharacterSelectionPanel : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonListView* SelectionListView;
	/********************/
};
