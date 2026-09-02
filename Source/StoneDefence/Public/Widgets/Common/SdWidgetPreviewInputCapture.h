// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cores/SimpleMove.h"
#include "Cores/SimpleRotate.h"
#include "Cores/SimpleZoom.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetPreviewInputCapture.generated.h"

class ASdPlayerControllerBase;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetPreviewInputCapture : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

public:
	USdWidgetPreviewInputCapture();

	void ConfigurePreviewInputCaptureWidget(AActor* InTargetActor);

protected:
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	SimpleActorAction::SimpleRotate* SimpleRotate;
	SimpleActorAction::SimpleZoom* SimpleZoom;
	SimpleActorAction::SimpleMove* SimpleMove;

	UPROPERTY(EditDefaultsOnly, Category="Zoom")
	float ZoomSpeed = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Move")
	float MoveSpeed = 0.2f;
};
