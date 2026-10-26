// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

/*
 * Tips:
 * 1. 控制台输入 AbilitySystem.DebugAbilityTags 可以看到视野中的单位所携带的tag
 * 2. 控制台输入 AbilitySystem.DebugAttribute Health MaxHealth (可以实时debug)
 */
namespace EventTags
{
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Event_Dead);

	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Event_SwitchCharacter);

	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Event_DrawWeapon);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Event_SheatheWeapon);
}
