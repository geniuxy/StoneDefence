// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Climb.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "GeniuxyDebugHelper.h"
#include "Interfaces/GeClimbProvider.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/Character.h"
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
	const IGeClimbProvider* ClimbProvider = Cast<IGeClimbProvider>(Avatar);

	if (!Character || !ClimbProvider)
	{
		Debug::Print(
			TEXT("[GA_Climb] Avatar 不是 ACharacter，或者没有实现 IGeClimbProvider（参数拿不到）"),
			TEXT("Climb.Prepare"), 8.f, FColor::Red);
		return false;
	}

	// 参数从接口拿，和角色门控用的是同一份，所以两端、两处判定不会分歧
	if (!GeClimb::ProbeClimb(
		Avatar->GetWorld(), GeClimb::MakeProbeInput(*Character),
		ClimbProvider->GetClimbProbeParams(), Avatar, CachedLedge))
	{
		// 门控已经拦过一次，能走到这里说明两端几何有偏差（或参数被改过），值得记一笔
		Debug::Print(
			TEXT("[GA_Climb] 门控通过但本端探不到可攀的墙，检查两端位置是否一致"),
			TEXT("Climb.Prepare"), 6.f, FColor::Orange);
		return false;
	}

	return true;
}

bool UGA_Climb::PreLoadMontage()
{
	ClimbStartMontage = LoadMontageByTag(ClimbStartMontageTag);

	if (!ClimbStartMontage)
	{
		// 起攀这一整段全靠这段蒙太奇：AN 加 tag、ANS 做对齐。
		// 没配好就没有「爬」可言，所以这里是 error 而不是 GA_Fly 那种「只告警、照样飞」。
		Debug::Print(
			FString::Printf(
				TEXT("[GA_Climb] 取不到 Tag %s 对应的起攀蒙太奇（AnimationSet 没配或加载失败）"),
				*ClimbStartMontageTag.ToString()),
			TEXT("Climb.PreLoad"), 8.f, FColor::Red);
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
	// 先把 warp 目标写好再播：ANS 在 notify 开始那一刻就要按名字读它
	ApplyClimbWarpTarget();

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

void UGA_Climb::ApplyClimbWarpTarget()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !CachedLedge.IsValid())
	{
		return;
	}

	UMotionWarpingComponent* WarpingComp = Avatar->FindComponentByClass<UMotionWarpingComponent>();
	if (!WarpingComp)
	{
		Debug::Print(
			TEXT("[GA_Climb] 角色身上没有 MotionWarpingComponent，起攀不会贴墙对齐（动画会在原地播）"),
			TEXT("Climb.Warp"), 8.f, FColor::Yellow);
		return;
	}

	WarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(
		ClimbWarpTargetName, CachedLedge.LedgeLocation, CachedLedge.ClimbForward.Rotation());
}

void UGA_Climb::HandleMontageEnded()
{
	// 起手那段结束了，本能力收工。
	// 注意：Ge.Stats.Climb **不在这里清** —— 它由蒙太奇里的 AN/GE 加、由「攀爬结束」
	// 那条路径（松手 / 到顶 / 离墙）负责清。这里清了的话攀爬状态会当场消失。
	Debug::Print(TEXT("[GA_Climb] 起攀蒙太奇结束，交给攀爬状态接管"),
	             TEXT("Climb.End"), 3.f, FColor::Cyan);

	// OnBlendOut / OnCompleted 可能都来，别结束两次
	if (IsActive())
	{
		K2_EndAbility();
	}
}
