// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceLocoClimb.h"

void UGeAnimInstanceLocoClimb::CacheDerivedState()
{
	Super::CacheDerivedState();

	constexpr float ClimbDeadZone = 0.15f;

	bIsClimbUp = LocalVelocity.Z > ClimbDeadZone;
	bIsClimbDown = LocalVelocity.Z < -ClimbDeadZone;
}
