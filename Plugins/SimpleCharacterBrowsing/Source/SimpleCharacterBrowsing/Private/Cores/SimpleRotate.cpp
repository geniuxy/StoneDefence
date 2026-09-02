// Fill out your copyright notice in the Description page of Project Settings.


#include "Cores/SimpleRotate.h"

namespace SimpleActorAction
{
	SimpleRotate::SimpleRotate(): Controller(nullptr)
	{
	}

	void SimpleRotate::Configure(AActor* InActor, APlayerController* InController)
	{
		Controller = InController;
		TargetActor = InActor;
	}

	void SimpleRotate::Tick(float DeltaTime)
	{
		if (!Controller || !TargetActor.IsValid()) return;

		if (bRotating)
		{
			USkeletalMeshComponent* Mesh = TargetActor->FindComponentByClass<USkeletalMeshComponent>();
			FRotator CurRot = Mesh ? Mesh->GetRelativeRotation() : TargetActor->GetActorRotation();
			FVector2D NewMousePos;
			Controller->GetMousePosition(NewMousePos.X, NewMousePos.Y);
			float RotSpeed = (NewMousePos.X - MousePos.X) * -1.f;

			CurRot.Yaw += RotSpeed * 1.f;
			if (Mesh)
			{
				Mesh->SetRelativeRotation(CurRot);
			}
			else
			{
				TargetActor->SetActorRotation(CurRot);
			}

			MousePos = NewMousePos;
		}
	}

	TStatId SimpleRotate::GetStatId() const
	{
		return TStatId();
	}

	void SimpleRotate::BeginRotate()
	{
		bRotating = true;
		Controller->GetMousePosition(MousePos.X, MousePos.Y);
	}

	void SimpleRotate::EndRotate()
	{
		bRotating = false;
		MousePos = FVector2D();
	}
}
