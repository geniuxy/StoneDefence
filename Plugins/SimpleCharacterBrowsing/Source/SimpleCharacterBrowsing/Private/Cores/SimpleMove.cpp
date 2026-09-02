// Fill out your copyright notice in the Description page of Project Settings.


#include "Cores/SimpleMove.h"

#include "Camera/CameraComponent.h"
#include "Cores/CameraUtils.h"

namespace SimpleActorAction
{
	SimpleMove::SimpleMove(): IntervalLength(0.f), MoveSpeed(0.f), bMoving(false)
	{
	}

	void SimpleMove::Tick(float DeltaTime)
	{
		if (!Controller || !TargetCamera.IsValid() || !TargetActor.IsValid()) return;

		if (bMoving)
		{
			FVector CurrentCamLoc = TargetCamera->GetComponentLocation();

			FVector2D NewMousePos;
			Controller->GetMousePosition(NewMousePos.X, NewMousePos.Y);

			float DeltaMouseX = NewMousePos.X - MousePos.X;
			float DeltaMouseY = NewMousePos.Y - MousePos.Y; // UE中鼠标往上，Y减小，反之，Y增大

			FVector Right = FVector::CrossProduct(RayDir, FVector::UpVector).GetSafeNormal();
			FVector PlaneUp = FVector::CrossProduct(Right, RayDir).GetSafeNormal();

			FVector DeltaWorld = Right * (DeltaMouseX * MoveSpeed) + PlaneUp * (DeltaMouseY * MoveSpeed);
			FVector DesiredCamLoc = CurrentCamLoc + DeltaWorld;

			FVector FinalCamLoc = DesiredCamLoc;
			if (bRayValid)
			{
				const float HalfRange = IntervalLength / 2.f;
				FinalCamLoc = CameraUtil::ClampMaxVerticalDistanceToRay(FinalCamLoc, RayOrigin, RayDir, HalfRange);
			}

			TargetCamera->SetWorldLocation(FinalCamLoc);
			MousePos = NewMousePos;
		}
	}

	TStatId SimpleMove::GetStatId() const
	{
		return TStatId();
	}

	void SimpleMove::Configure(
		AActor* InActor,
		APlayerController* InController,
		UCameraComponent* InTargetCamera,
		float InLength,
		float InMoveSpeed)
	{
		Controller = InController;
		TargetActor = InActor;
		TargetCamera = InTargetCamera;
		IntervalLength = InLength;
		MoveSpeed = InMoveSpeed;

		bRayValid = false;
		if (!InActor || !InTargetCamera) return;

		RayOrigin = InActor->GetActorLocation();
		RayDir = (RayOrigin - InTargetCamera->GetComponentLocation()).GetSafeNormal();
		if (!RayDir.IsZero())
		{
			bRayValid = true;
		}
	}

	void SimpleMove::BeginMove()
	{
		bMoving = true;
		Controller->GetMousePosition(MousePos.X, MousePos.Y);
	}

	void SimpleMove::EndMove()
	{
		bMoving = false;
		MousePos = FVector2D();
	}
}
