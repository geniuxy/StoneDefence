// Fill out your copyright notice in the Description page of Project Settings.


#include "Cores/SimpleZoom.h"

#include "Camera/CameraComponent.h"

namespace SimpleActorAction
{
	SimpleZoom::SimpleZoom(): MinDistance(0.f), MaxDistance(0.f)
	{
	}

	void SimpleZoom::Configure(
		AActor* InActor, UCameraComponent* InCameraComp, int32 InMinDistance, int32 InMaxDistance)
	{
		TargetActor = InActor;
		TargetCamera = InCameraComp;

		MinDistance = InMinDistance;
		MaxDistance = InMaxDistance;
	}

	void SimpleZoom::Zoom(float InValue)
	{
		if (!TargetActor.IsValid() || !TargetCamera.IsValid()) return;

		const FVector ActorLoc = TargetActor->GetActorLocation();
		const FVector CamLoc = TargetCamera->GetComponentLocation();

		// 当前相机指向目标的方向
		FVector DirToTarget = (ActorLoc - CamLoc).GetSafeNormal();
		if (DirToTarget.IsZero())
			return;

		// InValue：滚轮增量；沿着朝向目标方向偏移
		FVector DesiredCamLoc = CamLoc + DirToTarget * InValue;

		// 计算期望位置到目标的距离
		float DesiredDistance = FVector::Dist(DesiredCamLoc, ActorLoc);

		// 钳位距离
		float ClampedDistance = FMath::Clamp(DesiredDistance, MinDistance, MaxDistance);

		// 从目标反向，生成最终相机位置
		FVector FinalCamLoc = ActorLoc - DirToTarget * ClampedDistance;

		TargetCamera->SetWorldLocation(FinalCamLoc);
	}
}
