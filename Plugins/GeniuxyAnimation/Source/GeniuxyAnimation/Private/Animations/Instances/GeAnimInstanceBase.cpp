// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GeniuxyAnimationType.h"
#include "SimpleAdvancedAnimationBPLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Tags/StatsTags.h"

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
	
	OwnerPawn = TryGetPawnOwner();
	if (OwnerPawn)
	{
		if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn))
		{
			OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_InCombat).AddUObject(
				this, &ThisClass::OwnerCombatTagUpdated
			);
			OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_Fly).AddUObject(
				this, &ThisClass::OwnerFlyTagUpdated
			);
		}
	}
}

void UGeAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 游戏线程：Actor / MovementComponent 的数据只能在这里读，
	// 读出来缓存在成员里给动画工作线程用。
	if (OwnerPawn)
	{
		const FVector Velocity = OwnerPawn->GetVelocity();
		Speed = Velocity.Length();
	}
}

void UGeAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// CalcFootIKOffset();
}

void UGeAnimInstanceBase::OwnerCombatTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	ActionState = NewCount > 0 ? ECharacterActionState::COMBAT : ECharacterActionState::NORMAL;
}

void UGeAnimInstanceBase::OwnerFlyTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	ActionState = NewCount > 0 ? ECharacterActionState::FLIGHT : ECharacterActionState::NORMAL;
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
