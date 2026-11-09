// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

/*
 * Tips:
 * 1. 控制台输入 AbilitySystem.DebugAbilityTags 可以看到视野中的单位所携带的tag
 * 2. 控制台输入 AbilitySystem.DebugAttribute Health MaxHealth (可以实时debug)
 */
namespace AbilityTags
{
	/*
	 * 能力分组（Ability Group）。
	 *
	 * 用途：标在 GA 的 AbilityTags 上，表示「这条能力属于哪个运动状态」。
	 * 然后由「模式的持有者」（例如 GA_Fly）用 BlockAbilitiesWithTag / CancelAbilitiesWithTag
	 * 一键禁掉 / 打断整组旧状态的能力 —— 这就是「进新状态时 Remove 掉 OldMode 专属 GA」的
	 * 声明式等价物，不用真的 ClearAbility。
	 *
	 * 为什么不用真的 Give/Remove：
	 * - GiveAbility 只能在服务端调，spec 复制到 owning client 有 RTT，这期间客户端按键
	 *   会在 AbilityLocalInputPressed 里找不到 spec、被静默丢弃；
	 * - ClearAbility 会让正在跑的能力走 EndAbility(bWasCancelled=true) 中断路径，
	 *   动画/位移/GE 的还原全得改走中断分支，容易留脏状态。
	 *
	 * 生命周期：BlockAbilitiesWithTag / CancelAbilitiesWithTag 只在本能力存活期间生效，
	 * 本能力结束（例如 GA_Fly 落地结束）时自动解除 —— 所以不需要写反向代码。
	 *
	 * 注意命名：UGameplayAbility 自己有一个同名成员 AbilityTags（能力自身携带的 Tag 容器），
	 * 本命名空间和它只是重名、不冲突 —— 在 GA 的构造函数里写 AbilityTags::Ge_xxx 也能正常解析
	 * （限定名查找不看数据成员）。但读代码时容易混，留意一下。
	 */
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Ability_Group_Ground);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Ability_Group_Fly);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Ability_Group_Swim);
	GENIUXYCOMMON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ge_Ability_Group_Climb);
}
