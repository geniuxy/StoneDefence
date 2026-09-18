// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSets/GeAttributeSetHero.h"

#include "Net/UnrealNetwork.h"

void UGeAttributeSetHero::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetHero, Intelligence, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetHero, Strength, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetHero, Experience, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetHero, PrevLevelExperience, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetHero, NextLevelExperience, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetHero, Level, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetHero, UpgradePoint, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetHero, MaxLevel, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGeAttributeSetHero, MaxLevelExperience, COND_None, REPNOTIFY_Always);
}

void UGeAttributeSetHero::OnRep_Intelligence(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetHero, Intelligence, OldValue);
}

void UGeAttributeSetHero::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetHero, Strength, OldValue);
}

void UGeAttributeSetHero::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetHero, Experience, OldValue);
}

void UGeAttributeSetHero::OnRep_PrevLevelExperience(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetHero, PrevLevelExperience, OldValue);
}

void UGeAttributeSetHero::OnRep_NextLevelExperience(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetHero, NextLevelExperience, OldValue);
}

void UGeAttributeSetHero::OnRep_Level(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetHero, Level, OldValue);
}

void UGeAttributeSetHero::OnRep_UpgradePoint(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetHero, UpgradePoint, OldValue);
}

void UGeAttributeSetHero::OnRep_MaxLevel(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetHero, MaxLevel, OldValue);
}

void UGeAttributeSetHero::OnRep_MaxLevelExperience(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGeAttributeSetHero, MaxLevelExperience, OldValue);
}
