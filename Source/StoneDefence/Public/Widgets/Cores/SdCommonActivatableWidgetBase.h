// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SdCommonActivatableWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdCommonActivatableWidgetBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

	/**********************************************************************/
	/*                           Parent Widget                            */
	/**********************************************************************/
protected:
	UPROPERTY()
	UCommonUserWidget* ParentWidget;

public:
	template <class T = UCommonUserWidget>
	T* GetParentWidget() const { return Cast<T>(ParentWidget); }

	void SetParentWidget(UCommonUserWidget* InWidget);

protected:
	virtual void OnSetParentWidget()
	{
	}
};
