// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeniuxyGASType.h"
#include "Engine/DataAsset.h"

#include "PA_AbilitySystemGenerics.generated.h"

class UInputAction;
class UInputMappingContext;
class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UPA_AbilitySystemGenerics : public UPrimaryDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStatsEffect;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TSubclassOf<UGameplayEffect> DialogStatsEffect;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TSubclassOf<UGameplayEffect> DisableAimEffect;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities; // 被动的Abilities

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> CommonAbilities; // 通用的主动Abilities

	UPROPERTY(EditDefaultsOnly, Category="Base Stats")
	UDataTable* BaseStatDataTable;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TMap<EAbilityInputID, UInputAction*> GameplayAbilityInputActions;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* GameplayInputMappingContext;

public:
	FORCEINLINE TSubclassOf<UGameplayEffect> GetFullStatsEffect() const { return FullStatsEffect; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDialogStatsEffect() const { return DialogStatsEffect; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDeathEffect() const { return DeathEffect; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDisableAimEffect() const { return DisableAimEffect; }
	FORCEINLINE TArray<TSubclassOf<UGameplayAbility>> GetPassiveAbilities() const { return PassiveAbilities; }
	FORCEINLINE TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> GetCommonAbilities() const
	{
		return CommonAbilities;
	}

	FORCEINLINE UDataTable* GetBaseStatDataTable() const { return BaseStatDataTable; }
	FORCEINLINE TArray<TSubclassOf<UGameplayEffect>> GetInitialGameplayEffects() const
	{
		return InitialGameplayEffects;
	}

	FORCEINLINE UInputMappingContext* GetGameplayInputMappingContext() const { return GameplayInputMappingContext; }
	FORCEINLINE TMap<EAbilityInputID, UInputAction*> GetGameplayAbilityInputActions() const
	{
		return GameplayAbilityInputActions;
	}
};
