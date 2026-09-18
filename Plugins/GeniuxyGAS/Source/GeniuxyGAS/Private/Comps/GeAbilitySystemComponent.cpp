// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/GeAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "GeniuxyGASType.h"
#include "AttributeSets/GeAttributeSetBase.h"
#include "AttributeSets/GeAttributeSetHero.h"
#include "Datas/PA_AbilitySystemGenerics.h"
#include "Net/UnrealNetwork.h"
#include "Tags/EventTags.h"
#include "Tags/StatsTags.h"

UGeAbilitySystemComponent::UGeAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UGeAttributeSetBase::GetHealthAttribute()).AddUObject(
		this, &ThisClass::HealthUpdated
	);
	GetGameplayAttributeValueChangeDelegate(UGeAttributeSetBase::GetEnergyAttribute()).AddUObject(
		this, &ThisClass::EnergyUpdated
	);

	GenericConfirmInputID = (int32)EAbilityInputID::Confirm;
	GenericCancelInputID = (int32)EAbilityInputID::Cancel;
}

void UGeAbilitySystemComponent::ServerSideInit()
{
	InitializeBaseAttributes();
	InitializeBaseGameplayEffects();
	GiveInitialAbilities();
}

void UGeAbilitySystemComponent::ApplyFullStatsEffect()
{
	if (AbilitySystemGenerics && AbilitySystemGenerics->GetFullStatsEffect())
	{
		AuthApplyGameplayEffect(AbilitySystemGenerics->GetFullStatsEffect());
	}
}

void UGeAbilitySystemComponent::ApplyDialogStatsEffect()
{
	if (AbilitySystemGenerics && AbilitySystemGenerics->GetDialogStatsEffect())
	{
		AuthApplyGameplayEffect(AbilitySystemGenerics->GetDialogStatsEffect());
	}
}

void UGeAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

UInputMappingContext* UGeAbilitySystemComponent::GetInputMappingContext() const
{
	return AbilitySystemGenerics->GetGameplayInputMappingContext();
}

TMap<EAbilityInputID, UInputAction*> UGeAbilitySystemComponent::GetGameplayAbilityInputActions() const
{
	return AbilitySystemGenerics->GetGameplayAbilityInputActions();
}

void UGeAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGeAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, LockTarget);
}

void UGeAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!AbilitySystemGenerics || !AbilitySystemGenerics->GetBaseStatDataTable() ||
		!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	UDataTable* BaseStatDataTable = AbilitySystemGenerics->GetBaseStatDataTable();
	const FHeroBaseStats* BaseStats = nullptr;
	for (const TPair<FName, uint8*>& DataPair : BaseStatDataTable->GetRowMap())
	{
		BaseStats = BaseStatDataTable->FindRow<FHeroBaseStats>(DataPair.Key, "");
		if (BaseStats && BaseStats->HeroClass == GetOwner()->GetClass())
		{
			break;
		}
	}
	if (!BaseStats) return;

	SetNumericAttributeBase(UGeAttributeSetBase::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);
	SetNumericAttributeBase(UGeAttributeSetBase::GetMaxEnergyAttribute(), BaseStats->BaseMaxEnergy);
	SetNumericAttributeBase(UGeAttributeSetBase::GetAttackDamageAttribute(), BaseStats->BaseAttackDamage);
	SetNumericAttributeBase(UGeAttributeSetBase::GetArmorAttribute(), BaseStats->BaseArmor);
	SetNumericAttributeBase(UGeAttributeSetBase::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);
	SetNumericAttributeBase(UGeAttributeSetBase::GetMoveAccelerationAttribute(), BaseStats->BaseMoveAcceleration);
	SetNumericAttributeBase(UGeAttributeSetBase::GetWeightLimitAttribute(), BaseStats->BaseWeightLimit);
	SetNumericAttributeBase(UGeAttributeSetBase::GetGoldAttribute(), BaseStats->BaseGold);

	if (GetAttributeSubobject(UGeAttributeSetHero::StaticClass()))
	{
		SetNumericAttributeBase(UGeAttributeSetHero::GetStrengthAttribute(), BaseStats->Strength);
		SetNumericAttributeBase(UGeAttributeSetHero::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowthRate);
		SetNumericAttributeBase(UGeAttributeSetHero::GetIntelligenceAttribute(), BaseStats->Intelligence);
		SetNumericAttributeBase(
			UGeAttributeSetHero::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate
		);
	}
}

void UGeAbilitySystemComponent::InitializeBaseGameplayEffects()
{
	if (!AbilitySystemGenerics || !GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : AbilitySystemGenerics->GetInitialGameplayEffects())
	{
		AuthApplyGameplayEffect(EffectClass);
	}
}

void UGeAbilitySystemComponent::GiveInitialAbilities()
{
	if (!AbilitySystemGenerics || !GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TPair<EAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr));
	}

	for (const TPair<EAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, (int32)AbilityPair.Key, nullptr));
	}

	for (const TSubclassOf<UGameplayAbility>& PassiveAbility : AbilitySystemGenerics->GetPassiveAbilities())
	{
		GiveAbility(FGameplayAbilitySpec(PassiveAbility, 1, -1, nullptr));
	}

	for (const TPair<EAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : AbilitySystemGenerics->GetCommonAbilities())
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, (int32)AbilityPair.Key, nullptr));
	}
}

void UGeAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AbilitySystemGenerics || !GetOwner() || !GetOwner()->HasAuthority()) return;

	bool bFound = false;
	float MaxHealth = GetGameplayAttributeValue(UGeAttributeSetBase::GetMaxHealthAttribute(), bFound);
	if (bFound && ChangeData.NewValue >= MaxHealth)
	{
		if (!HasMatchingGameplayTag(StatsTags::Ge_Stats_Health_Full))
		{
			// 只在本地执行Tag添加
			AddLooseGameplayTag(StatsTags::Ge_Stats_Health_Full);
		}
	}
	else
	{
		RemoveLooseGameplayTag(StatsTags::Ge_Stats_Health_Full);
	}

	if (ChangeData.NewValue <= 0.f)
	{
		if (!HasMatchingGameplayTag(StatsTags::Ge_Stats_Health_Empty))
		{
			AddLooseGameplayTag(StatsTags::Ge_Stats_Health_Empty);

			// 这一步给角色施加了Dk_Stats_Dead的Tag
			if (AbilitySystemGenerics->GetDeathEffect())
			{
				AuthApplyGameplayEffect(AbilitySystemGenerics->GetDeathEffect());
			}

			FGameplayEventData DeadAbilityEventData;
			if (ChangeData.GEModData)
			{
				DeadAbilityEventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetContext();
			}

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				GetOwner(), EventTags::Ge_Event_Dead, DeadAbilityEventData
			);
		}
	}
	else
	{
		RemoveLooseGameplayTag(StatsTags::Ge_Stats_Health_Empty);
	}
}

void UGeAbilitySystemComponent::EnergyUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AbilitySystemGenerics || !GetOwner() || !GetOwner()->HasAuthority()) return;

	bool bFound = false;
	float MaxMana = GetGameplayAttributeValue(UGeAttributeSetBase::GetMaxEnergyAttribute(), bFound);
	if (bFound && ChangeData.NewValue >= MaxMana)
	{
		if (!HasMatchingGameplayTag(StatsTags::Ge_Stats_Energy_Full))
		{
			// 只在本地执行Tag添加
			AddLooseGameplayTag(StatsTags::Ge_Stats_Energy_Full);
		}
	}
	else
	{
		RemoveLooseGameplayTag(StatsTags::Ge_Stats_Energy_Full);
	}

	if (ChangeData.NewValue <= 0.f)
	{
		if (!HasMatchingGameplayTag(StatsTags::Ge_Stats_Energy_Empty))
		{
			AddLooseGameplayTag(StatsTags::Ge_Stats_Energy_Empty);
		}
	}
	else
	{
		RemoveLooseGameplayTag(StatsTags::Ge_Stats_Energy_Empty);
	}
}

