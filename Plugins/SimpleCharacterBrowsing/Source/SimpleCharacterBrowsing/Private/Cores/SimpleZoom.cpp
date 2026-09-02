// Fill out your copyright notice in the Description page of Project Settings.


#include "Cores/SimpleZoom.h"

#include "Camera/CameraComponent.h"

namespace SimpleActorAction
{
	SimpleZoom::SimpleZoom(): IntervalLength(0)
	{
	}

	void SimpleZoom::Configure(AActor* InActor, UCameraComponent* InCameraComp, int32 InLength)
	{
		TargetActor = InActor;
		IntervalLength = InLength;
		TargetCamera = InCameraComp;

		if (!TargetActor.IsValid() || !TargetCamera.IsValid())
		{
			return;
		}

		CameraLocation = TargetCamera->GetComponentLocation();
		DirToTargetActor = (TargetActor->GetActorLocation() - CameraLocation).GetSafeNormal();

		NearestCameraLocation = CameraLocation + DirToTargetActor * IntervalLength / 2.f;
		FarthestCameraLocation = CameraLocation - DirToTargetActor * IntervalLength / 2.f;
	}

	void SimpleZoom::Zoom(float InValue)
	{
		if (!TargetActor.IsValid() || !TargetCamera.IsValid()) return;

		// 沿着朝向目标方向偏移相机
		FVector NewCameraLocation = TargetCamera->GetComponentLocation() + InValue * DirToTargetActor;

		// 把NewCameraLocation投影到DirToTargetActor射线上，再限制在最近最远两点之间
		const float ProjNear  = FVector::DotProduct(NearestCameraLocation, DirToTargetActor);
		const float ProjFar   = FVector::DotProduct(FarthestCameraLocation, DirToTargetActor);
		const float ProjNew   = FVector::DotProduct(NewCameraLocation, DirToTargetActor);

		const float ClampedProj = FMath::Clamp(ProjNew, ProjFar, ProjNear);

		// 还原回世界坐标位置
		FVector ClampedLocation = (ClampedProj * DirToTargetActor) + (TargetActor->GetActorLocation() - DirToTargetActor
			* FVector::DotProduct(TargetActor->GetActorLocation(), DirToTargetActor));

		// 设置相机组件位置
		TargetCamera->SetWorldLocation(ClampedLocation);
	}
}
