// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Climb.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Interfaces/GeStateEvaluator.h"
#include "Tags/AbilityTags.h"
#include "Tags/EventTags.h"
#include "Tags/StatsTags.h"

UGA_Climb::UGA_Climb()
{
	// 入口一：角色门控通过后按 Tag 激活（见 ASdCharacterHeroBase::HandleJumpInput）
	// 注意 AbilityTags:: 指的是命名空间（和 UGameplayAbility 那个同名成员不冲突）
	AbilityTags.AddTag(AbilityTags::Ge_Ability_Climb_Start);

	// 入口二：事件驱动。留给下一步的「跳起抓边」和以后的「被击飞贴墙」复用同一个入口
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = EventTags::Ge_Event_Climb_Start;
	AbilityTriggers.Add(TriggerData);

	// 已经在爬、或者已经死了，就别再起一次
	ActivationBlockedTags.AddTag(StatsTags::Ge_Stats_Climbing);
	ActivationBlockedTags.AddTag(StatsTags::Ge_Stats_Dead);

	// 起手这一段占住地面能力；起手结束、能力收工后由 Ge.Stats.Climb 接着挡
	// （GA_Walk / GA_Sprint 已经把 Ge.Stats.Climb 写进自己的 ActivationBlockedTags）
	CancelAbilitiesWithTag.AddTag(AbilityTags::Ge_Ability_Group_Ground);
	BlockAbilitiesWithTag.AddTag(AbilityTags::Ge_Ability_Group_Ground);

	// 客户端预测 + 服务器执行：两端都会跑 PrepareActivation，各自探一次
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGA_Climb::PrepareActivation()
{
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	const ACharacter* Character = Cast<ACharacter>(Avatar);
	const IGeStateEvaluator* StateEvaluator = Cast<IGeStateEvaluator>(Avatar);

	if (!Character || !StateEvaluator)
	{
		return false;
	}

	if (!StateEvaluator->CanStartClimb())
	{
		return false;
	}

	return true;
}

bool UGA_Climb::PreLoadMontage()
{
	ClimbStartMontage = LoadMontageByTag(ClimbStartMontageTag);

	if (!ClimbStartMontage)
	{
		return false;
	}

	return true;
}

void UGA_Climb::OnActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 起攀要让动画自带的 RootMotion 生效（MotionWarping 就是在它的基础上纠偏），
	// 所以这里**不要**像 GA_Land 那样传 AnimRootMotionTranslationScale = 0。
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ClimbStartMontage);

	if (!MontageTask)
	{
		K2_EndAbility();
		return;
	}

	// 四种收尾都走同一个回调：起手段结束就交还给攀爬状态（tag / 模式 / PhysCustom）
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::HandleMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::HandleMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::HandleMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::HandleMontageEnded);
	MontageTask->ReadyForActivation();
}

void UGA_Climb::HandleMontageEnded()
{
	// 起手那段结束了，本能力收工。
	// 注意：Ge.Stats.Climb **不在这里清** —— 它由蒙太奇里的 AN/GE 加、由「攀爬结束」
	// 那条路径（松手 / 到顶 / 离墙）负责清。这里清了的话攀爬状态会当场消失。

	// OnBlendOut / OnCompleted 可能都来，别结束两次
	if (IsActive())
	{
		K2_EndAbility();
	}
}
