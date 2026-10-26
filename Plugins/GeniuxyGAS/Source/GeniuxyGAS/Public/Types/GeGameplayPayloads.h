// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeGameplayPayloads.generated.h"

class UGameplayAbility;

/**
 * ============================================================================
 *  GeniuxyGAS 的 GameplayEvent 载荷 / 包装 UObject 集中放这里。
 * ============================================================================
 */

/**
 * UGA_SwitchCharacter -> 当前角色 的 GameplayEvent 载荷。
 *
 * 塞进 FGameplayEventData::OptionalObject，接收方 Cast 出来读。
 */
UCLASS(BlueprintType)
class GENIUXYGAS_API USwitchCharacterPayload : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * 要切换到的队伍槽位（1-based，也就是"第几个队友"）。
	 *
	 * 注意不是 EAbilityInputID 的原始值：UGA_SwitchCharacter 已经把触发它的
	 * InputKey 换算成槽位了（按键 1 -> 槽位 1）。
	 */
	UPROPERTY(BlueprintReadWrite, Category="Switch Character")
	int32 SlotIndex = INDEX_NONE;

	/** 发起这次切换的 Ability，方便接收方回查 / 打断 */
	UPROPERTY(BlueprintReadWrite, Category="Switch Character")
	TObjectPtr<UGameplayAbility> SourceAbility = nullptr;
};
