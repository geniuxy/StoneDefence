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

		void Configure(AActor* InActor, UCameraComponent* InCameraComp, int32 InLength);

		void Zoom(float InValue);

	protected:
		TWeakObjectPtr<AActor> TargetActor;
		TWeakObjectPtr<UCameraComponent> TargetCamera;
		int32 IntervalLength;
		FVector CameraLocation;
		FVector DirToTargetActor;
		FVector NearestCameraLocation;
		FVector FarthestCameraLocation;
	};
}
