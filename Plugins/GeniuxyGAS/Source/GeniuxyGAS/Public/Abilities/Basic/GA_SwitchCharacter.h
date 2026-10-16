// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "GameplayTagContainer.h"
#include "GeniuxyGASType.h"
#include "GA_SwitchCharacter.generated.h"

/**
 * 切换当前出战角色。
 *
 * 这个 Ability 只做"转发 + 换算"，不碰任何游戏规则：
 *   1. 从 FGameplayAbilitySpec::InputID 取出触发它的那个 InputKey；
 *   2. 把 InputKey 换算成 1-based 的队伍槽位 SlotIndex；
 *   3. 把 SlotIndex 打包成 GameplayEvent 发给当前角色（AvatarActor）；
 *   4. 角色自己决定怎么换人（销毁自己、生成新 Pawn、交给控制器……）。
 *
 * 关于换算 —— 这是最容易踩的坑：
 * EAbilityInputID 的枚举值和"第几个队友"不是一回事：
 *   None=0, BasicAttack=1, Aim=2, AbilityOne=3, AbilityTwo=4, AbilityThree=5 ...
 * 玩家按的 1/2/3/4 键绑的是 AbilityOne..AbilityFour，枚举值是 3/4/5/6。
 * 所以默认按 "InputID - FirstSlotInputKey + 1" 换算：
 *   按键 1(AbilityOne=3) -> 槽位 1
 *   按键 2(AbilityTwo=4) -> 槽位 2
 * 输入映射不一样的话改 FirstSlotInputKey 就行。
 *
 * 为什么拆成两半：
 * UGA_SwitchCharacter 住在 GeniuxyGAS 插件里，插件不能反向依赖 StoneDefence 游戏模块，
 * 所以"销毁自己 + 生成新角色"这种强项目相关的逻辑放在 ASdCharacterHeroBase 里，
 * 由它监听 EventTags::Ge_Event_SwitchCharacter。
 *
 * 使用前提：
 * 这个 Ability 必须通过 EAbilityInputID 授权，也就是在
 * UGeAbilitySystemComponent::GiveInitialAbilities() 那种
 * GiveAbility(FGameplayAbilitySpec(AbilityClass, Level, (int32)InputID, nullptr)) 的方式给，
 * 否则 InputID 是 INDEX_NONE，拿不到 InputKey。
 */
UCLASS()
class GENIUXYGAS_API UGA_SwitchCharacter : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_SwitchCharacter();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	FGameplayTag SwitchCharacterEventTag;

	/**
	 * "第 1 个队友"对应的 InputKey。
	 *
	 * 默认 AbilityOne：键盘 1 键 -> 槽位 1，键盘 2 键 -> 槽位 2……
	 * 注意这里填的是 EAbilityInputID 的枚举项，不是键盘数字。
	 */
	UPROPERTY(EditDefaultsOnly, Category="Switch Character")
	EAbilityInputID FirstSlotInputKey = EAbilityInputID::AbilityOne;
};
