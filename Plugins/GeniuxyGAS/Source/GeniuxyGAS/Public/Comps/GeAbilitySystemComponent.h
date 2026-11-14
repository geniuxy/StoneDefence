// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GeniuxyGASType.h"
#include "Interfaces/GeStateProvider.h"
#include "GeAbilitySystemComponent.generated.h"

class UGameplayEffect;
class UInputAction;
class UInputMappingContext;
class UPA_AbilitySystemGenerics;

UCLASS(ClassGroup=(GAS), meta=(BlueprintSpawnableComponent))
class GENIUXYGAS_API UGeAbilitySystemComponent : public UAbilitySystemComponent, public IGeStateProvider
{
	GENERATED_BODY()

public:
	UGeAbilitySystemComponent();

	void ServerSideInit();

	void ApplyFullStatsEffect();
	void ApplyDialogStatsEffect();

	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);

	UInputMappingContext* GetInputMappingContext() const;
	const TMap<EAbilityInputID, UInputAction*>& GetGameplayAbilityInputActions() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void InitializeBaseAttributes();
	void InitializeBaseGameplayEffects();
	void GiveInitialAbilities();
	
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void EnergyUpdated(const FOnAttributeChangeData& ChangeData);

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TArray<FGeAbilitySpecEntry> Abilities; // 可学习的Abilities（初始0级）

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TArray<FGeAbilitySpecEntry> BasicAbilities; // 初始Abilities（初始1级）

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	UPA_AbilitySystemGenerics* AbilitySystemGenerics;

public:
	FORCEINLINE const TArray<FGeAbilitySpecEntry>& GetAbilities() const { return Abilities; }

	/**********************************************************************/
	/*                             Targeting                              */
	/**********************************************************************/
private:
	UPROPERTY(Replicated)
	TObjectPtr<AActor> LockTarget;

public:
	FORCEINLINE AActor* GetLockTarget() const { return LockTarget; }
	void SetLockTarget(AActor* NewLockTarget) { LockTarget = NewLockTarget; }

	/**********************************************************************/
	/*                         Ground Locomotion                          */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, Category = "GAS|Ground Locomotion")
	virtual bool IsGroundWalking() const override { return bGroundWalking; }

	UFUNCTION(BlueprintPure, Category = "GAS|Ground Locomotion")
	virtual bool IsGroundSprinting() const override { return bGroundSprinting; }

private:
	bool bGroundWalking = false;
	bool bGroundSprinting = false;

	FDelegateHandle GroundWalkingTagHandle;
	FDelegateHandle GroundSprintingTagHandle;

	void HandleGroundWalkingTagChanged(const FGameplayTag Tag, int32 NewCount);
	void HandleGroundSprintingTagChanged(const FGameplayTag Tag, int32 NewCount);

	/**********************************************************************/
	/*                               Combat                               */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, Category = "GAS|Combat")
	virtual bool IsInCombat() const override { return bInCombat; }

private:
	bool bInCombat = false;

	FDelegateHandle InCombatTagHandle;

	void HandleInCombatTagChanged(const FGameplayTag Tag, int32 NewCount);

	/**********************************************************************/
	/*                                Swim                                */
	/**********************************************************************/
	/**
	 * 游泳状态的统一收尾：Ge.Stats.Swimming 计数归零（出水）时，
	 * 取消整组游泳能力（AbilityTags::Ge_Ability_Group_Swim）
	 */
private:
	FDelegateHandle SwimmingTagHandle;

	void HandleSwimmingTagChanged(const FGameplayTag Tag, int32 NewCount);

	/**********************************************************************/
	/*                          Cached Animation                          */
	/**********************************************************************/
private:
	UPROPERTY()
	TMap<FGameplayTag, TSoftObjectPtr<UAnimMontage>> MontageMap;

	bool bMontagesLoaded = false;

public:
	void UpdateMontageMap(FGameplayTag InTag, const TSoftObjectPtr<UAnimMontage>& InMontage);
	void PreLoadMontages();
	void OnMontagesLoaded();

	FORCEINLINE const TMap<FGameplayTag, TSoftObjectPtr<UAnimMontage>>& GetMontageMap() const { return MontageMap; }
	FORCEINLINE bool AreMontagesLoaded() const { return bMontagesLoaded; }
};
