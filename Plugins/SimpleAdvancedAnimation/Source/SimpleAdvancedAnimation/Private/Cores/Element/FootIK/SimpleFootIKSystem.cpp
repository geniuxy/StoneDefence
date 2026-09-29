#include "Cores/Element/FootIK/SimpleFootIKSystem.h"

void FFootIKSystem::Tick(float DeltaTime)
{
	for (auto& Tmp : FootIKs)
	{
		Tmp.Value.Tick(DeltaTime);
	}

	TArray<FSAAHandle> RemoveHandle;
	for (auto& Tmp : FootIKs)
	{
		if (Tmp.Value.IsPendingKill())
		{
			RemoveHandle.Add(Tmp.Key);
		}
	}

	for (auto& Tmp : RemoveHandle)
	{
		FootIKs.Remove(Tmp);
	}
}

FSAAHandle FFootIKSystem::CreateFootIK(
	ACharacter* InCharacter,
	const TArray<FName>& InBoneNames,
	float InTraceDistance,
	float InInterpSpeed,
	float InTraceStart)
{
	FSAAHandle Handle = FMath::RandRange(0, 999999);
	if (!FootIKs.Contains(Handle))
	{
		FSimpleFootIK& InFootIK = FootIKs.Add(Handle, FSimpleFootIK());
		InFootIK.Init(InCharacter, InBoneNames, InTraceDistance, InInterpSpeed, InTraceStart);

		return Handle;
	}

	return CreateFootIK(InCharacter, InBoneNames, InTraceDistance, InInterpSpeed);
}

FSimpleFootIK* FFootIKSystem::FindFootIK(const FSAAHandle InKey)
{
	return FootIKs.Find(InKey);
}

float FFootIKSystem::FindOffset(const FSAAHandle InKey, const FName& InKeyName)
{
	if (FFootIKInfo* InInfo = FindFootIKInfo(InKey, InKeyName))
	{
		return InInfo->Offset;
	}

	return 0.0f;
}

FFootIKInfo* FFootIKSystem::FindFootIKInfo(const FSAAHandle InKey, const FName& InKeyName)
{
	if (FSimpleFootIK* InFootIK = FindFootIK(InKey))
	{
		return InFootIK->FindFootIKInfo(InKeyName);
	}

	return nullptr;
}
