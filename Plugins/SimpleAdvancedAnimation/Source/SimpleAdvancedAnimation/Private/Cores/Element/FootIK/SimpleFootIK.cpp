#include "Cores/Element/FootIK/SimpleFootIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

FSimpleFootIK::FSimpleFootIK()
	:Character(nullptr)
	,TraceDistance(50.f)
	,InterpSpeed(18.f)
	,TraceStart(50.f)
	,bPendingKill(false)
{
}

float FSimpleFootIK::FindOffset(const FName& InKeyName)
{
	if (FFootIKInfo *Info = FindFootIKInfo(InKeyName))
	{
		return Info->Offset;
	}

	return 0.f;
}

void FSimpleFootIK::Init(
	ACharacter* InCharacter, 
	const TArray<FName>& InBoneNames, 
	float InTraceDistance, 
	float InInterpSpeed,
	float InTraceStart)
{
	Character = InCharacter;
	TraceDistance = InTraceDistance;
	InterpSpeed = InInterpSpeed;
	TraceStart = InTraceStart;
	for (auto &Tmp : InBoneNames)
	{
		IKInfos.Add(Tmp,FFootIKInfo());
	}
}

FFootIKInfo*FSimpleFootIK::FindFootIKInfo(const FName& InKeyName)
{
	return IKInfos.Find(InKeyName);
}

void FSimpleFootIK::Tick(float DeltaTime)
{
	for (auto &Tmp : IKInfos)
	{
		float OffsetTarget = FootTrace(Tmp.Key,TraceDistance);
		Tmp.Value.Offset = FMath::FInterpTo(Tmp.Value.Offset, OffsetTarget, DeltaTime, InterpSpeed);
	}
}

float FSimpleFootIK::FootTrace(const FName& BoneName, float InTraceDistance)
{
	if (Character.IsValid())
	{
		if (Character->IsPendingKillPending())
		{
			bPendingKill = true;
			Character = nullptr;
			return 0.f;
		}

		//end
		float CharacterLocationZ = Character->GetActorLocation().Z;
		float HalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		//Start
		FVector StartLocation = Character->GetMesh()->GetSocketLocation(BoneName) + FVector(0.f, 0.f,TraceStart);
		FVector EndLocation = FVector(StartLocation.X,StartLocation.Y,CharacterLocationZ - (HalfHeight + InTraceDistance));
	
		FHitResult HitResult;
		TArray<AActor*> Ignores;
		if (UKismetSystemLibrary::LineTraceSingle(
			Character->GetWorld(),
			StartLocation,
			EndLocation,
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			Ignores,
			EDrawDebugTrace::Type::ForOneFrame,
			HitResult,
			true))
		{
			return (HitResult.Location - HitResult.TraceEnd).Size() - InTraceDistance;
		}
	}

	return 0.0f;
}
