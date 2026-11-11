// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/Modifier/GE_Swim_Sprint_Speed.h"

#include "AttributeSets/GeAttributeSetBase.h"

namespace
{
	constexpr float SwimSprintSpeedMultiplier = 2.f;
}

UGE_Swim_Sprint_Speed::UGE_Swim_Sprint_Speed()
{
	AddMultiplicativeModifier(UGeAttributeSetBase::GetSwimSpeedAttribute(), SwimSprintSpeedMultiplier);
}
