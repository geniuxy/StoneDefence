// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

/*
 * Tips:
 * 1. 控制台输入 AbilitySystem.DebugAbilityTags 可以看到视野中的单位所携带的tag
 * 2. 控制台输入 AbilitySystem.DebugAttribute Health MaxHealth (可以实时debug)
 */
namespace StatsTags
{
	GENIUXYGAS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Stats_Health_Full);
	GENIUXYGAS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Stats_Health_Empty);
	GENIUXYGAS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Stats_Dead);
	GENIUXYGAS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Stats_Energy_Full);
	GENIUXYGAS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Stats_Energy_Empty);
	GENIUXYGAS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Stats_InCombat);
	GENIUXYGAS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Stats_CanAttack);
	GENIUXYGAS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Stats_EquippingSword);
	// GENIUXYGAS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Stats_Equipped_PrimaryWeapon);
}
