// Fill out your copyright notice in the Description page of Project Settings.


#include "Cores/SimpleMove.h"

#include "Camera/CameraComponent.h"

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
			float DeltaMouseY = NewMousePos.Y - MousePos.Y; // UE屏幕中，向上Y减小;向下Y增大

			// 第一步：把鼠标屏幕差值转为世界位移（需要构建垂直固定射线的正交基底）
			FVector Right = FVector::CrossProduct(RayDir, FVector::UpVector).GetSafeNormal();
			FVector PlaneUp = FVector::CrossProduct(Right, RayDir).GetSafeNormal();

			FVector DeltaWorld = Right * (DeltaMouseX * MoveSpeed) + PlaneUp * (DeltaMouseY * MoveSpeed);
			FVector DesiredCamLoc = CurrentCamLoc + DeltaWorld;

			// ========== 点到射线分解 ==========
			// V：从射线原点指向期望相机位置
			FVector V = DesiredCamLoc - RayOrigin;

			// 平行于射线的投影（沿着射线方向，保留不动）
			float ProjParallel = FVector::DotProduct(V, RayDir);
			FVector ParallelComponent = RayDir * ProjParallel;

			// 垂直射线的分量（这就是相机偏离射线的偏移，要限制它的长度）
			FVector PerpendicularComponent = V - ParallelComponent;

			const float HalfRange = IntervalLength / 2.f;
			// 钳位垂直分量的长度，不能超过HalfRange
			if (PerpendicularComponent.SizeSquared() > HalfRange * HalfRange)
			{
				PerpendicularComponent = PerpendicularComponent.GetSafeNormal() * HalfRange;
			}

			// 重组最终相机位置
			// 射线原点 + 平行分量(沿射线) + 钳位后的垂直分量
			FVector FinalCamLoc = RayOrigin + ParallelComponent + PerpendicularComponent;

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
		if (!InActor || !InTargetCamera)
			return;

		// 快照：Configure时刻的射线
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
