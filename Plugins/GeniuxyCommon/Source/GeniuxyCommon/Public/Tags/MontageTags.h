// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

/*
 * Tips:
 * 1. 控制台输入 AbilitySystem.DebugAbilityTags 可以看到视野中的单位所携带的tag
 * 2. 控制台输入 AbilitySystem.DebugAttribute Health MaxHealth (可以实时debug)
 */
namespace MontageTags
{
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_DrawWeapon);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_SheatheWeapon);

	/* 地面四向闪避：前 / 后 / 左 / 右 */
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Dodge_Forward);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Dodge_Backward);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Dodge_Left);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Dodge_Right);

	/* 飞行四向闪避：上 / 下 / 左 / 右（对应 AM_Flight_Dodge_*_U / _D / _L / _R） */
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Dodge_Fly_Up);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Dodge_Fly_Down);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Dodge_Fly_Left);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Dodge_Fly_Right);

	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Fly_HoverStart);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Montage_Fly_FastStart);
}
