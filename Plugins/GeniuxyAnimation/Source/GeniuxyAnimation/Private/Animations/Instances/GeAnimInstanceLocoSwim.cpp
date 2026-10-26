// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceLocoSwim.h"

void UGeAnimInstanceLocoSwim::CacheDerivedState()
{
	Super::CacheDerivedState();

	constexpr float DiveDeadZone = 0.2f;

	bIsDiving = LocalVelocity.Z < -DiveDeadZone;
}
