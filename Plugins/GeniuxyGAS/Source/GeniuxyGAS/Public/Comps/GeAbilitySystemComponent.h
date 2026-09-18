// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GeAbilitySystemComponent.generated.h"

class UGameplayEffect;
class UInputAction;
class UInputMappingContext;
class UPA_AbilitySystemGenerics;
enum class EAbilityInputID : uint8;

UCLASS(ClassGroup=(GAS), meta=(BlueprintSpawnableComponent))
class GENIUXYGAS_API UGeAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGeAbilitySystemComponent();

	void ServerSideInit();

	void ApplyFullStatsEffect();
	void ApplyDialogStatsEffect();

	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);

	UInputMappingContext* GetInputMappingContext() const;
	TMap<EAbilityInputID, UInputAction*> GetGameplayAbilityInputActions() const;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void InitializeBaseAttributes();
	void InitializeBaseGameplayEffects();
	void GiveInitialAbilities();
	
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void EnergyUpdated(const FOnAttributeChangeData& ChangeData);

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities; // 可学习的Abilities（初始0级）

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities; // 初始Abilities（初始1级）

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	UPA_AbilitySystemGenerics* AbilitySystemGenerics;

public:
	FORCEINLINE const TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }

	/**********************************************************************/
	/*                             Targeting                              */
	/**********************************************************************/

private:
	UPROPERTY(Replicated)
	TObjectPtr<AActor> LockTarget;

public:
	FORCEINLINE AActor* GetLockTarget() const { return LockTarget; }
	void SetLockTarget(AActor* NewLockTarget) { LockTarget = NewLockTarget; }
};
