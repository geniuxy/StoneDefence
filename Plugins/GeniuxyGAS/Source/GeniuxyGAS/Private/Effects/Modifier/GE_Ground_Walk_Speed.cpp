// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/Modifier/GE_Ground_Walk_Speed.h"

#include "AttributeSets/GeAttributeSetBase.h"

namespace
{
	constexpr float GroundWalkSpeedMultiplier = 0.25f;
}

UGE_Ground_Walk_Speed::UGE_Ground_Walk_Speed()
{
	AddMultiplicativeModifier(UGeAttributeSetBase::GetMoveSpeedAttribute(), GroundWalkSpeedMultiplier);
}
