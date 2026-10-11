// Fill out your copyright notice in the Description page of Project Settings.


#include "Robots/SdRobotBase.h"


SdRobotBase::SdRobotBase()
{
}

void SdRobotBase::Tick(float DeltaTime)
{
	GateRobot.Tick(DeltaTime);
	DsRobot.Tick(DeltaTime);
}

void SdRobotBase::InitGate(const FString& Host, const int32 Port)
{
	GateRobot.Init(Host, Port);
}

void SdRobotBase::InitDs(const FString& Host, const int32 Port)
{
	DsRobot.Init(Host, Port);
}
