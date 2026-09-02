// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"

/**
 * 
 */
namespace SimpleActorAction
{
	class SIMPLECHARACTERBROWSING_API SimpleRotate : public FTickableGameObject
	{
	public:
		SimpleRotate();

		void Configure(AActor* InActor, APlayerController* InController);

		virtual void Tick(float DeltaTime) override;
		virtual TStatId GetStatId() const override;

		void BeginRotate();
		void EndRotate();

	protected:
		bool bRotating = false;
		APlayerController* Controller;
		TWeakObjectPtr<AActor> TargetActor;
		FVector2D MousePos;
	};
}
