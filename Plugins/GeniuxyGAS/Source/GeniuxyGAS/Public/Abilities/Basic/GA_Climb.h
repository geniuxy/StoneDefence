// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "Tags/MontageTags.h"
#include "GA_Climb.generated.h"

class UAnimMontage;

/**
 * 攀爬起手（原地起攀）。
 *
 * 职责边界（很重要）：
 * - 本能力**只管「起攀那一小段」**：探测 → 对齐（warp）→ 播起攀蒙太奇 → 结束。
 * - 攀爬状态本身不归它：montage 里的 AN（AN_ApplyGameplayEffect + 带 GrantedTags 的 GE）
 *   会在指定帧加上 Ge.Stats.Climb，UGeLocomotionComponent 监听到它就把移动模式切到
 *   Climb（MOVE_Custom / MOVE_Climb），循环攀爬由 PhysCustom + AnimBP 接手。
 * - 所以 **不要**把 Ge.Stats.Climb 写进 ActivationOwnedTags：能力一结束 tag 就会被回收，
 *   人会在起攀动画播完的瞬间掉回 Ground，循环阶段根本接不上。
 *
 * 入口有两个（两者可以并存）：
 * 1) 角色门控通过后 TryActivateAbilitiesByTag(AbilityTags = Ge.Ability.Climb.Start)；
 * 2) 发 GameplayEvent(Ge.Event.Climb.Start) —— 留给下一步的「跳起抓边」/「被击飞贴墙」。
 *
 * 探测为什么在能力里再做一次：
 * 能力是 LocalPredicted，客户端和服务器都会跑一遍。客户端要立刻播（预测），服务器要权威，
 * 而 SendGameplayEvent 携带的 payload 不会跨网络（ServerTryActivateAbility 不带 EventData），
 * 所以两端各自用同一份参数（IGeClimbProvider）各探一次，结果天然一致。
 */
UCLASS()
class GENIUXYGAS_API UGA_Climb : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Climb();

	/** 前置准备：本端探测目标 edge（失败 = 这次不激活，基类会 K2_EndAbility） */
	virtual bool PrepareActivation() override;

	/** 预加载起攀蒙太奇（攀爬起始没有蒙太奇就没意义，这里失败直接不激活） */
	virtual bool PreLoadMontage() override;

	virtual void OnActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	/** 起攀蒙太奇播完 / 被打断：起手段结束，交还给攀爬状态 */
	UFUNCTION()
	void HandleMontageEnded();

	/**********************************************************************/
	/*                              Montage                               */
	/**********************************************************************/
	/** 起攀蒙太奇 Tag（MontageMap 里的 Key，在 AnimationSet 配具体资产） */
	UPROPERTY(EditDefaultsOnly, Category = "Climb|Montage", meta = (Categories = "Ge.Montage"))
	FGameplayTag ClimbStartMontageTag = MontageTags::Ge_Montage_Climb_Start;

	UPROPERTY()
	UAnimMontage* ClimbStartMontage = nullptr;

	/**********************************************************************/
	/*                               Warp                                 */
	/**********************************************************************/
	/**
	 * MotionWarping 的目标名。
	 * 必须和蒙太奇里 AnimNotifyState_MotionWarping 上配的名字完全一致，
	 * 否则 warp 静默失效（和「Slot 名对不上」是同一类坑）。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Climb|Warp")
	FName ClimbWarpTargetName = TEXT("ClimbLedge");
};
