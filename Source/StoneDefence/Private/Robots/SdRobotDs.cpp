// Fill out your copyright notice in the Description page of Project Settings.


#include "Robots/SdRobotDs.h"

#include "SimpleNetManage.h"


SdRobotDs::SdRobotDs()
{
}

void SdRobotDs::Tick(float DeltaTime)
{
	if (DsClient)
	{
		DsClient->Tick(DeltaTime);
	}
}

void SdRobotDs::Init(const FString& Host, const int32 Port)
{
}
