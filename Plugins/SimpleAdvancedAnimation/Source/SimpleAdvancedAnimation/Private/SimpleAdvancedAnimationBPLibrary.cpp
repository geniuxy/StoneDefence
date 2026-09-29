// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleAdvancedAnimationBPLibrary.h"
#include "Cores/SimpleAdvancedAnimSystem.h"

USimpleAdvancedAnimationBPLibrary::USimpleAdvancedAnimationBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

int32 USimpleAdvancedAnimationBPLibrary::CreateFootIK(
	ACharacter* InCharacter,
	const TArray<FName>& InBoneNames,
	float TraceDistance,
	float InterpSpeed,
	float InTraceStart)
{
	return FSimpleAdvancedAnimSystem::Get()->GetFootIKSystem()->CreateFootIK(
		InCharacter, InBoneNames, TraceDistance, InterpSpeed, InTraceStart
	);
}

float USimpleAdvancedAnimationBPLibrary::FindOffset(const int32 InKey, const FName& InKeyName)
{
	return FSimpleAdvancedAnimSystem::Get()->GetFootIKSystem()->FindOffset(InKey, InKeyName);
}

FFootIKInfo USimpleAdvancedAnimationBPLibrary::FindFootIKInfo(const int32 InKey, const FName& InKeyName)
{
	if (FFootIKInfo* InFootIKInfo = FSimpleAdvancedAnimSystem::Get()->GetFootIKSystem()->FindFootIKInfo(
		InKey, InKeyName))
	{
		return *InFootIKInfo;
	}

	return FFootIKInfo();
}

float USimpleAdvancedAnimationBPLibrary::GetButtZOffset(const TArray<float>& InOffsets)
{
	float TmpValue = 0.f;
	for (auto& Tmp : InOffsets)
	{
		TmpValue = FMath::Min(TmpValue, Tmp);
	}

	return TmpValue < 0.f ? TmpValue : 0.f;
}

void USimpleAdvancedAnimationBPLibrary::Destroy()
{
	FSimpleAdvancedAnimSystem::Destroy();
}
