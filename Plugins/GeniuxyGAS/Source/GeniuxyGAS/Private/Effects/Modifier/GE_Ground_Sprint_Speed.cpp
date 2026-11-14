// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/Modifier/GE_Ground_Sprint_Speed.h"

#include "AttributeSets/GeAttributeSetBase.h"

namespace
{
	constexpr float GroundSprintSpeedMultiplier = 1.5f;
}

UGE_Ground_Sprint_Speed::UGE_Ground_Sprint_Speed()
{
	AddMultiplicativeModifier(UGeAttributeSetBase::GetMoveSpeedAttribute(), GroundSprintSpeedMultiplier);
}
