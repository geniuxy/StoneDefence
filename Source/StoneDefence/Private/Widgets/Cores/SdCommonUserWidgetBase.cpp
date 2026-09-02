// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Cores/SdCommonUserWidgetBase.h"

void USdCommonUserWidgetBase::SetParentWidget(USdCommonUserWidgetBase* InWidget)
{
	ParentWidget = InWidget;
	OnSetParentWidget();
}
