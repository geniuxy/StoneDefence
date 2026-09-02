// Fill out your copyright notice in the Description page of Project Settings.


#include "Cores/SimpleZoom.h"

#include "Camera/CameraComponent.h"
#include "Cores/CameraUtils.h"

namespace SimpleActorAction
{
	SimpleZoom::SimpleZoom(): IntervalLength(0.f), MinDistance(0.f), MaxDistance(0.f)
	{
	}

	void SimpleZoom::Configure(
		AActor* InActor,
		UCameraComponent* InCameraComp,
		float InLength,
		float InMinDistance,
		float InMaxDistance)
	{
		TargetActor = InActor;
		TargetCamera = InCameraComp;
		IntervalLength = InLength;
		MinDistance = InMinDistance;
		MaxDistance = InMaxDistance;

		bRayValid = false;
		RayOrigin = FVector::ZeroVector;
		RayDir = FVector::ZeroVector;

		if (!InActor || !InCameraComp) return;

		RayOrigin = InActor->GetActorLocation();
		RayDir = (RayOrigin - InCameraComp->GetComponentLocation()).GetSafeNormal();
		if (!RayDir.IsZero())
		{
			bRayValid = true;
		}
	}

	void SimpleZoom::Zoom(float InValue)
	{
		if (!TargetActor.IsValid() || !TargetCamera.IsValid()) return;

		const FVector ActorLoc = TargetActor->GetActorLocation();
		const FVector CamLoc = TargetCamera->GetComponentLocation();

		FVector DirToCamera = (CamLoc - ActorLoc).GetSafeNormal();
		if (DirToCamera.IsZero()) return;

		FVector Offset = CamLoc - ActorLoc;

		float ProjParallel = FVector::DotProduct(Offset, DirToCamera);
		FVector Parallel = DirToCamera * ProjParallel;
		FVector Vertical = Offset - Parallel;

		float NewParallelLen = ProjParallel - InValue;
		float ClampedLen = FMath::Clamp(NewParallelLen, MinDistance, MaxDistance);
		FVector NewParallel = DirToCamera * ClampedLen;

		FVector FinalCamLoc = ActorLoc + NewParallel + Vertical;

		if (bRayValid)
		{
			const float HalfRange = IntervalLength / 2.f;
			FinalCamLoc = CameraUtil::ClampMaxVerticalDistanceToRay(FinalCamLoc, RayOrigin, RayDir, HalfRange);
		}

		TargetCamera->SetWorldLocation(FinalCamLoc);
	}
}
