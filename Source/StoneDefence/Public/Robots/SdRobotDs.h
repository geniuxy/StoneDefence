// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FSimpleNetManage;
/**
 * 
 */
class STONEDEFENCE_API SdRobotDs
{
public:
	SdRobotDs();

	void Tick(float DeltaTime);

	void Init(const FString& Host, const int32 Port);

private:
	FSimpleNetManage* DsClient = nullptr;
};
