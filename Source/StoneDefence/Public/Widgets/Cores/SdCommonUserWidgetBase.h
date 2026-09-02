// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SdCommonUserWidgetBase.generated.h"

class FSimpleChannel;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdCommonUserWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
	{
	}

	/**********************************************************************/
	/*                           Parent Widget                            */
	/**********************************************************************/
protected:
	UPROPERTY()
	USdCommonUserWidgetBase* ParentWidget;

public:
	template <class T = USdCommonUserWidgetBase>
	T* GetParentWidget() const { return Cast<T>(ParentWidget); }

	void SetParentWidget(USdCommonUserWidgetBase* InWidget);

protected:
	virtual void OnSetParentWidget() {}
};
