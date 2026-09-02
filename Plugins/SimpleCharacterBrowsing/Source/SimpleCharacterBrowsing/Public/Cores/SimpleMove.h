// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UCameraComponent;
/**
 * 
 */
namespace SimpleActorAction
{
	class SIMPLECHARACTERBROWSING_API SimpleMove : public FTickableGameObject
	{
	public:
		SimpleMove();

		virtual void Tick(float DeltaTime);
		virtual TStatId GetStatId() const;

		void Configure(
			AActor* InActor,
			APlayerController* InController,
			UCameraComponent* InTargetCamera,
			float InLength,
			float InMoveSpeed
		);

		void BeginMove();
		void EndMove();

	protected:
		TWeakObjectPtr<AActor> TargetActor;
		TWeakObjectPtr<UCameraComponent> TargetCamera;
		APlayerController* Controller = nullptr;

		FVector RayOrigin;
		FVector RayDir;
		bool bRayValid = false;

		float IntervalLength;
		float MoveSpeed;
		bool bMoving;
		FVector2D MousePos;
	};
}
