// Fill out your copyright notice in the Description page of Project Settings.


#include "Tags/AbilityTags.h"

namespace AbilityTags
{
	/* 能力分组：标在 GA 的 AbilityTags 上，表示「我属于哪个运动状态」 */
	UE_DEFINE_GAMEPLAY_TAG(Ge_Ability_Group_Ground, "Ge.Ability.Group.Ground");
	UE_DEFINE_GAMEPLAY_TAG(Ge_Ability_Group_Fly, "Ge.Ability.Group.Fly");
	UE_DEFINE_GAMEPLAY_TAG(Ge_Ability_Group_Swim, "Ge.Ability.Group.Swim");
	UE_DEFINE_GAMEPLAY_TAG(Ge_Ability_Group_Climb, "Ge.Ability.Group.Climb");
}