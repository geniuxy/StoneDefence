// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GeniuxyGAS.h"
#include "GeAttributeSetHero.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGeAttributeSetHero : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, Intelligence)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, Strength)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, Experience)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, PrevLevelExperience)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, NextLevelExperience)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, Level)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, UpgradePoint)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, MaxLevel)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, MaxLevelExperience)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, IntelligenceGrowthRate)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetHero, StrengthGrowthRate)
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Intelligence)
	FGameplayAttributeData Intelligence;

	UPROPERTY(ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;

	UPROPERTY()
	FGameplayAttributeData IntelligenceGrowthRate;

	UPROPERTY()
	FGameplayAttributeData StrengthGrowthRate;

	UPROPERTY(ReplicatedUsing = OnRep_Experience)
	FGameplayAttributeData Experience;

	UPROPERTY(ReplicatedUsing = OnRep_PrevLevelExperience)
	FGameplayAttributeData PrevLevelExperience;

	UPROPERTY(ReplicatedUsing = OnRep_NextLevelExperience)
	FGameplayAttributeData NextLevelExperience;

	UPROPERTY(ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;

	UPROPERTY(ReplicatedUsing = OnRep_UpgradePoint)
	FGameplayAttributeData UpgradePoint;

	UPROPERTY(ReplicatedUsing = OnRep_MaxLevel)
	FGameplayAttributeData MaxLevel;

	UPROPERTY(ReplicatedUsing = OnRep_MaxLevelExperience)
	FGameplayAttributeData MaxLevelExperience;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PrevLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_NextLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_UpgradePoint(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxLevel(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxLevelExperience(const FGameplayAttributeData& OldValue);
};
