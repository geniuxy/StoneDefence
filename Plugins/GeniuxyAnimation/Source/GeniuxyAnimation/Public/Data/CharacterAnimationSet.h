// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GeniuxyAnimationType.h"
#include "Engine/DataAsset.h"
#include "CharacterAnimationSet.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYANIMATION_API UCharacterAnimationSet : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, FCharacterMontageEntry> Montages;

	const FCharacterMontageEntry* FindEntry(FGameplayTag ActionTag) const;
};
