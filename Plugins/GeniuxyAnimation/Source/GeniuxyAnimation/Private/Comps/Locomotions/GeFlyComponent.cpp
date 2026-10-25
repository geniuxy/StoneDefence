// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/Locomotions/GeFlyComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/StatsTags.h"

UGeFlyComponent::UGeFlyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGeFlyComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_Fly).AddUObject(
			this, &ThisClass::OwnerFlyTagUpdated
		);
	}
	OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
}

void UGeFlyComponent::OwnerFlyTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (!OwnerMovementComp) return;
	if (NewCount > 0)
	{
		OwnerMovementComp->SetMovementMode(MOVE_Flying);
	}
	else
	{
		OwnerMovementComp->SetMovementMode(MOVE_Walking);
	}
}

