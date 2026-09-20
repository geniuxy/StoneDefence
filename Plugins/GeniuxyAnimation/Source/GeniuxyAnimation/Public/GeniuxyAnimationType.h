#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GeniuxyAnimationType.generated.h"

USTRUCT(BlueprintType)
struct FCharacterMontageEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ActionTag;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly)
	FName StartSection;

	UPROPERTY(EditDefaultsOnly)
	float PlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredTags;
};