// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstanceBase.h"

#include "SimpleAdvancedAnimationBPLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void USdAnimInstanceBase::NativeInitializeAnimation()
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

void USdAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// CalcFootIKOffset();
}

void USdAnimInstanceBase::InitFootIKId(ACharacter* OwnerCharacter)
{
	BoneNames.Empty();
	if (GetIsFootIK())
	{
		BoneNames.Add(LeftBoneName);
		BoneNames.Add(RightBoneName);

		FootIKId = USimpleAdvancedAnimationBPLibrary::CreateFootIK(OwnerCharacter, BoneNames);
	}
}

void USdAnimInstanceBase::CalcFootIKOffset()
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

float USdAnimInstanceBase::GetFootIKOffset(const FName& InBoneName)
{
	if (FootIKId != INDEX_NONE)
	{
		return USimpleAdvancedAnimationBPLibrary::FindOffset(FootIKId,InBoneName);
	}

	return 0.0f;
}
