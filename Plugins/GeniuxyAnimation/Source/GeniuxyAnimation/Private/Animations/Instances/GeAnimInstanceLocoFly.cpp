// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceLocoFly.h"

#include "GeniuxyDebugHelper.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGeAnimInstanceLocoFly::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UGeAnimInstanceLocoFly::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn))
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		const FRotator CharacterRot = OwnerCharacter->GetActorRotation();
		Velocity = CharacterRot.UnrotateVector(Velocity);
		if (UCharacterMovementComponent* InCharacterMovementComp =
			Cast<UCharacterMovementComponent>(OwnerCharacter->GetMovementComponent()))
		{
			float MaxFlySpeed = InCharacterMovementComp->MaxFlySpeed;

			FlySpeed.X = FMath::GetMappedRangeValueClamped(
				FVector2D(-MaxFlySpeed, MaxFlySpeed), FVector2D(-1,1), Velocity.X
			);
			FlySpeed.Y = FMath::GetMappedRangeValueClamped(
				FVector2D(-MaxFlySpeed, MaxFlySpeed), FVector2D(-1,1), Velocity.Y
			);
			FlySpeed.Z = FMath::GetMappedRangeValueClamped(
				FVector2D(-MaxFlySpeed, MaxFlySpeed), FVector2D(-1,1), Velocity.Z
			);

			Debug::Print(TEXT("X飞行速度"), (float)FlySpeed.X);
			Debug::Print(TEXT("Y飞行速度"), (float)FlySpeed.Y);
		}
	}
}

void UGeAnimInstanceLocoFly::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}
