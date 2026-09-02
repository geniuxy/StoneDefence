// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UCameraComponent;
/**
 * 
 */
namespace SimpleActorAction
{
	class SIMPLECHARACTERBROWSING_API SimpleZoom
	{
	public:
		SimpleZoom();

		void Configure(
			AActor* InActor,
			UCameraComponent* InCameraComp,
			float InLength,
			float InMinDistance,
			float InMaxDistance
		);

		void Zoom(float InValue);

	protected:
		TWeakObjectPtr<AActor> TargetActor;
		TWeakObjectPtr<UCameraComponent> TargetCamera;

		FVector RayOrigin;
		FVector RayDir;
		bool bRayValid = false;

		float IntervalLength;
		float MinDistance;
		float MaxDistance;
	};
}
