// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Common/SdWidgetPreviewInputCapture.h"

#include "Camera/CameraComponent.h"
#include "Controllers/PlayerControllers/SdPlayerControllerBase.h"

USdWidgetPreviewInputCapture::USdWidgetPreviewInputCapture()
{
	SimpleRotate = new SimpleActorAction::SimpleRotate();
	SimpleZoom = new SimpleActorAction::SimpleZoom();
}

void USdWidgetPreviewInputCapture::ConfigurePreviewInputCaptureWidget(AActor* InTargetActor)
{
	AActor* TargetPreviewActor = InTargetActor;
	ASdPlayerControllerBase* CachedPlayerController = GetOwningPlayer<ASdPlayerControllerBase>();
	if (TargetPreviewActor && CachedPlayerController)
	{
		UCameraComponent* CameraComp = TargetPreviewActor->FindComponentByClass<UCameraComponent>();
		SimpleRotate->Configure(TargetPreviewActor, CachedPlayerController);
		SimpleZoom->Configure(TargetPreviewActor, CameraComp, 200.f);
	}
}

void USdWidgetPreviewInputCapture::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	SimpleRotate->EndRotate();
}

FReply USdWidgetPreviewInputCapture::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		SimpleRotate->BeginRotate();
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		
	}
	return FReply::Handled();
}

FReply USdWidgetPreviewInputCapture::NativeOnMouseButtonUp(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		SimpleRotate->EndRotate();
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		
	}
	return FReply::Handled();
}

FReply USdWidgetPreviewInputCapture::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const float ZoomValue = InMouseEvent.GetWheelDelta();
	SimpleZoom->Zoom(ZoomValue * ZoomSpeed);
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}
