// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Cores/GeCommonActivatableWidgetBase.h"

void UGeCommonActivatableWidgetBase::SetParentWidget(UCommonUserWidget* InWidget)
{
	ParentWidget = InWidget;
	OnSetParentWidget();
}
