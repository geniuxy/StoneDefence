// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceBase.h"

#include "SimpleAdvancedAnimationBPLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void UGeAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerSkeletalMeshComp = GetSkelMeshComponent();
	bFootIK = bCanFootIK;
	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerSkeletalMeshComp->GetOwner());
	if (OwnerCharacter)
	{
		// InitFootIKId(OwnerCharacter);
	}
}

void UGeAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// CalcFootIKOffset();
}

void UGeAnimInstanceBase::InitFootIKId(ACharacter* OwnerCharacter)
{
	BoneNames.Empty();
	if (GetIsFootIK())
	{
		BoneNames.Add(LeftBoneName);
		BoneNames.Add(RightBoneName);

		FootIKId = USimpleAdvancedAnimationBPLibrary::CreateFootIK(OwnerCharacter, BoneNames);
	}
}

void UGeAnimInstanceBase::CalcFootIKOffset()
{
	if (GetIsFootIK() && FootIKId != INDEX_NONE)
	{
		TArray<float> OffsetArray;

		float LOffset = GetFootIKOffset(LeftBoneName);
		float ROffset = GetFootIKOffset(RightBoneName);

		OffsetArray.Add(LOffset);
		OffsetArray.Add(ROffset);

		ButtZOffset = USimpleAdvancedAnimationBPLibrary::GetButtZOffset(OffsetArray);

		LeftOffset = -(ButtZOffset - LOffset);
		RightOffset = ButtZOffset - ROffset;
	}
}

float UGeAnimInstanceBase::GetFootIKOffset(const FName& InBoneName)
{
	if (FootIKId != INDEX_NONE)
	{
		return USimpleAdvancedAnimationBPLibrary::FindOffset(FootIKId,InBoneName);
	}

	return 0.0f;
}
