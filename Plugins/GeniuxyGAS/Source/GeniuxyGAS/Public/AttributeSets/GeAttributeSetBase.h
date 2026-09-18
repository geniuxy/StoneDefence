// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GeniuxyGAS.h"
#include "GeAttributeSetBase.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYGAS_API UGeAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, Health);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, MaxHealth);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, CachedHealthPercent)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, Energy);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, MaxEnergy);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, CachedEnergyPercent)
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, AttackDamage);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, Armor);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, MoveSpeed);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, MoveAcceleration);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, CarryWeight);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, WeightLimit);
	ATTRIBUTE_ACCESSORS(UGeAttributeSetBase, Gold)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 用于GameplayEffect的Modifiers相关计算Clamp
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// 用于GameplayEffect的Executions相关计算Clamp
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	void RescaleHealth();
	void RescaleEnergy();
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy;
	UPROPERTY(ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy;
	UPROPERTY(ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	UPROPERTY(ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	UPROPERTY(ReplicatedUsing = OnRep_MoveAcceleration)
	FGameplayAttributeData MoveAcceleration;
	UPROPERTY(ReplicatedUsing = OnRep_CarryWeight)
	FGameplayAttributeData CarryWeight;
	UPROPERTY(ReplicatedUsing = OnRep_WeightLimit)
	FGameplayAttributeData WeightLimit;
	UPROPERTY(ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;
	
	UPROPERTY()
	FGameplayAttributeData CachedHealthPercent;
	UPROPERTY()
	FGameplayAttributeData CachedEnergyPercent;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Energy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxEnergy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MoveAcceleration(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CarryWeight(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_WeightLimit(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldValue);
};
