// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdRobotDs.h"
#include "SdRobotGate.h"
#include "StoneDefenceNetCommonType.h"

/**
 * 
 */
class STONEDEFENCE_API SdRobotBase
{
public:
	SdRobotBase();

	void Tick(float DeltaTime);

	void InitGate(const FString& Host, const int32 Port);
	void InitDs(const FString& Host, const int32 Port);

	SdRobotGate& GetGateRobot() { return GateRobot;}

private:
	SdRobotGate GateRobot;
	SdRobotDs DsRobot;

	FSdUserData UserData;

	/**********************************************************************/
	/*                           Run Request                              */
	/**********************************************************************/
public:
	void RunLoginToDsServerRequests();
};
