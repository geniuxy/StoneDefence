// Copyright (C) RenZhai.2020.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleAdvancedAnimationType.h"

struct FSimpleFootIK
{
public:
	FSimpleFootIK();

	void Init(ACharacter* InCharacter,const TArray<FName> &InBoneNames,float InTraceDistance = 50.f, float InInterpSpeed = 18.f,float InTraceStart = 50.f);

	void Tick(float DeltaTime);

	float FindOffset(const FName &InKeyName);
	FFootIKInfo *FindFootIKInfo(const FName& InKeyName);

	bool IsPendingKill() { return bPendingKill; }
protected:
	float FootTrace(const FName &BoneName,float InTraceDistance);
protected:
	TWeakObjectPtr<ACharacter> Character;
	TMap<FName,FFootIKInfo> IKInfos;
	float TraceDistance;
	float InterpSpeed;
	float TraceStart;
	bool bPendingKill;
};