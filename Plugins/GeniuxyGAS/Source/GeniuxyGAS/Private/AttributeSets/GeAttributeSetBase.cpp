// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSets/GeAttributeSetBase.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UGeAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, MoveAcceleration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, CarryWeight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, WeightLimit, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetBase, Gold, COND_None, REPNOTIFY_Always);
}

void UGeAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}
	if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxEnergy());
	}
}

void UGeAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	const FGameplayEffectContextHandle& Context = Data.EffectSpec.GetContext();
	AActor* EffectCauser = Context.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
		SetCachedHealthPercent(GetHealth() / GetMaxHealth());
	}
	if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
	{
		SetEnergy(FMath::Clamp(GetEnergy(), 0, GetMaxEnergy()));
		SetCachedEnergyPercent(GetEnergy() / GetMaxEnergy());
	}
}

void UGeAttributeSetBase::RescaleHealth()
{
	if (!GetOwningActor() || !GetOwningActor()->HasAuthority()) return;

	if (GetCachedHealthPercent() != 0.f && GetHealth() != 0.f)
	{
		SetHealth(FMath::Clamp(GetMaxHealth() * GetCachedHealthPercent(), 0, GetMaxHealth()));
	}
}

void UGeAttributeSetBase::RescaleEnergy()
{
	if (!GetOwningActor() || !GetOwningActor()->HasAuthority()) return;

	if (GetCachedEnergyPercent() != 0.f && GetEnergy() != 0.f)
	{
		SetEnergy(FMath::Clamp(GetMaxEnergy() * GetCachedEnergyPercent(), 0, GetMaxEnergy()));
	}
}

/*
 * GAMEPLAYATTRIBUTE_REPNOTIFY会校验和修正Client的预测值，修正会Server值
 */
void UGeAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, Health, OldValue);
}

void UGeAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, MaxHealth, OldValue);
}

void UGeAttributeSetBase::OnRep_Energy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, Energy, OldValue);
}

void UGeAttributeSetBase::OnRep_MaxEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, MaxEnergy, OldValue);
}

void UGeAttributeSetBase::OnRep_AttackDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, AttackDamage, OldValue);
}

void UGeAttributeSetBase::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, Armor, OldValue);
}

void UGeAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, MoveSpeed, OldValue);
}

void UGeAttributeSetBase::OnRep_MoveAcceleration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, MoveAcceleration, OldValue);
}

void UGeAttributeSetBase::OnRep_CarryWeight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, CarryWeight, OldValue);
}

void UGeAttributeSetBase::OnRep_WeightLimit(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, WeightLimit, OldValue);
}

void UGeAttributeSetBase::OnRep_Gold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetBase, Gold, OldValue);
}
