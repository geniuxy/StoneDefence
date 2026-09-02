// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Common/SdWidgetPreviewInputCapture.h"

#include "Controllers/PlayerControllers/SdPlayerControllerBase.h"

USdWidgetPreviewInputCapture::USdWidgetPreviewInputCapture()
{
	SimpleRotate = new SimpleActorAction::SimpleRotate();
}

void USdWidgetPreviewInputCapture::ConfigurePreviewInputCaptureWidget(AActor* InTargetActor)
{
	AActor* TargetPreviewActor = InTargetActor;
	ASdPlayerControllerBase* CachedPlayerController = GetOwningPlayer<ASdPlayerControllerBase>();

	SimpleRotate->Configure(TargetPreviewActor, CachedPlayerController);
}

void USdWidgetPreviewInputCapture::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	SimpleRotate->EndRotate();
}

FReply USdWidgetPreviewInputCapture::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SimpleRotate->BeginRotate();
	return FReply::Handled();
}

FReply USdWidgetPreviewInputCapture::NativeOnMouseButtonUp(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SimpleRotate->EndRotate();
	return FReply::Handled();
}

FReply USdWidgetPreviewInputCapture::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const float Direction = InMouseEvent.GetWheelDelta();
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}
