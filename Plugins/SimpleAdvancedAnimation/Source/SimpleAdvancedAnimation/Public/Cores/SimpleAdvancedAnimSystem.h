// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "Element/FootIK/SimpleFootIKSystem.h"

/**
 * 
 */
class FSimpleAdvancedAnimSystem : public FTickableGameObject
{
public:
	virtual void Tick(float DeltaTime);
	virtual TStatId GetStatId() const;

	static FSimpleAdvancedAnimSystem* Get();
	static void Destroy();

public:
	FFootIKSystem* GetFootIKSystem() { return &FootIKSystem; }
protected:
	static FSimpleAdvancedAnimSystem* AdvancedAnimSystem;

	FFootIKSystem FootIKSystem;
};
