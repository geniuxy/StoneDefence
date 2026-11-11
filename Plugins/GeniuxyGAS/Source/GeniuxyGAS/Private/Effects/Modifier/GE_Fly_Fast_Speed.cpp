// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/Modifier/GE_Fly_Fast_Speed.h"

#include "AttributeSets/GeAttributeSetBase.h"

namespace
{
	constexpr float FlyFastSpeedMultiplier = 2.f;
}

UGE_Fly_Fast_Speed::UGE_Fly_Fast_Speed()
{
	AddMultiplicativeModifier(UGeAttributeSetBase::GetFlySpeedAttribute(), FlyFastSpeedMultiplier);
}
