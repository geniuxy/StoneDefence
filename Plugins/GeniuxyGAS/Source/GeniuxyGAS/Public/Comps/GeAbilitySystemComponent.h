// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GeniuxyGASType.h"
#include "Interfaces/GeCombatStateProvider.h"
#include "GeAbilitySystemComponent.generated.h"

class UGameplayEffect;
class UInputAction;
class UInputMappingContext;
class UPA_AbilitySystemGenerics;

UCLASS(ClassGroup=(GAS), meta=(BlueprintSpawnableComponent))
class GENIUXYGAS_API UGeAbilitySystemComponent : public UAbilitySystemComponent, public IGeCombatStateProvider
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
	/*                               Combat                               */
	/**********************************************************************/
	/*
	 * 战斗状态的唯一监听点。
	 *
	 * 为什么在这里而不是 UGeLocomotionComponent：
	 * - Ge.Stats.InCombat 是 GAS 的 tag 语义（由 GE 授予 / 摘除），监听放在 GAS 侧才自然；
	 * - 以前挂在移动模式组件上，导致「动画想要战斗状态」也得先找到移动组件，职责越界；
	 * - 集中成一个 bool + 一个委托后，动画、UI、能力、音效都从同一处取，不会各监听一份。
	 *
	 * 复制：不需要额外 Replicated 字段。tag 本身随 GE（ActiveGameplayEffects）复制，
	 * 各端在 tag 计数变化时各自算一次 bInCombat。
	 *
	 * 对外读取走 IGeCombatStateProvider（接口在 GeniuxyCommon）：
	 * 动画侧 Cast 到那个接口读缓存值，所以 GeniuxyAnimation 不需要依赖本插件。
	 */
	UFUNCTION(BlueprintPure, Category = "GAS|Combat")
	virtual bool IsInCombat() const override { return bInCombat; }

private:
	/** 战斗状态缓存（= 目标身上有没有 Ge.Stats.InCombat） */
	bool bInCombat = false;

	FDelegateHandle InCombatTagHandle;

	/** Ge.Stats.InCombat 的 tag 计数变化回调（全项目唯一监听点） */
	void HandleInCombatTagChanged(const FGameplayTag Tag, int32 NewCount);

	/**********************************************************************/
	/*                                Swim                                */
	/**********************************************************************/
	/**
	 * 游泳状态的统一收尾：Ge.Stats.Swimming 计数归零（出水）时，
	 * 取消整组游泳能力（AbilityTags::Ge_Ability_Group_Swim）。
	 *
	 * 为什么放在 ASC，而不是每个能力自己挂 UAbilityTask_WaitGameplayTagRemoved：
	 * - GAS 的 ActivationRequiredTags 只决定「能不能激活」，不会取消已经激活的能力，
	 *   所以出水后 GA_Swim_Sprint / GA_Swim_Dive 会带着 tag 和速度 GE 留在岸上；
	 * - 一个能力挂一份 task = 同样的代码抄 N 遍，以后加新的游泳能力漏挂一次就是脏状态；
	 *   集中到 ASC 只有一处，新能力只要打上 Group.Swim 就自动被兜住。
	 *
	 * 触发时机：RegisterGameplayTagEvent 默认是 NewOrRemoved，只在 0 ↔ N 变化时回调，
	 * 所以这里判断 NewCount == 0 就是「刚刚摘干净」。
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
