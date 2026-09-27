// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstanceCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/StatsTags.h"

void USdAnimInstanceCharacterBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_InCombat).AddUObject(
			this, &ThisClass::OwnerCombatTagUpdated
		);
	}
}

void USdAnimInstanceCharacterBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter)
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();
	}

	if (OwnerMovementComp)
	{
		bIsInAir = OwnerMovementComp->IsFalling();
	}
}

void USdAnimInstanceCharacterBase::OwnerCombatTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	bIsInCombat = NewCount != 0;
}
