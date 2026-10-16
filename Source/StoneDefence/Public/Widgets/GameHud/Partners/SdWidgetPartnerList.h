// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "Widgets/Cores/GeCommonUserWidgetBase.h"
#include "SdWidgetPartnerList.generated.h"

class ASdCharacterHeroBase;
class UCommonListView;
/**
 * 队伍列表。
 *
 * 列表内容不在本地 New，而是完全由 ASdPlayerStateGameBase::PartnerInfos 驱动
 * （服务端权威 + 复制下来）：
 *   - 绑定到 PlayerState 后先按当前状态建一次（数据可能在 widget 创建前就复制过来了）；
 *   - 之后 PartnerInfos 一变（服务端直接广播 / 客户端 OnRep）就整体重建。
 *
 * 注意：GameHud 是在 ASdPlayerControllerGameBase::BeginPlay 里创建的，
 * 而客户端上那时 PlayerState 还没复制到位（AController::PlayerState 是
 * replicatedUsing=OnRep_PlayerState，只在客户端复制时才触发）。
 * 所以这里不能只试一次，要等 PlayerState 到齐再绑定。
 */
UCLASS()
class STONEDEFENCE_API USdWidgetPartnerList : public UGeCommonUserWidgetBase
{
	GENERATED_BODY()

public:
	/**
	 * 请求把一个角色加进队伍。
	 *
	 * @param InSlotIndex 队伍槽位（1-based，也就是"第几个队友"）；
	 *                    默认 INDEX_NONE 表示由 Server 自动取下一个空槽位。
	 *                    注意这里填的是队伍槽位，不是 EAbilityInputID 的枚举值 ——
	 *                    InputKey -> 槽位 的换算由 UGA_SwitchCharacter 负责（按键 1 -> 槽位 1）。
	 *                    另外不要自己算槽位，客户端的 PartnerInfos 是复制来的、会滞后。
	 */
	void AddCharacter(FGameplayTag InCharacterTag, bool bSelected = true, int32 InSlotIndex = INDEX_NONE);

protected:
	//~Begin UUserWidget UFunction
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End UUserWidget UFunction

private:
	UPROPERTY(meta=(BindWidget))
	UCommonListView* CharacterList;

	/**
	 * 尝试绑定到 ASdPlayerStateGameBase。
	 * PlayerState 还没复制到位就挂个重复定时器等它，绑定成功就停掉。
	 */
	void TryBindPlayerState();

	/** 用 PlayerState 里复制下来的 PartnerInfos 整体重建列表 */
	void RebuildPartnerList();

	/** PartnerInfos 变化回调（绑定到 ASdPlayerStateGameBase::OnPartnerInfosChanged） */
	UFUNCTION()
	void HandlePartnerInfosChanged();

	/** 等待 PlayerState 的重复定时器 */
	FTimerHandle BindPlayerStateTimerHandle;

	/** 已经成功绑定到 PlayerState */
	bool bPlayerStateBound = false;

	/** 重试次数，防止 PlayerState 一直不来时无限轮询 */
	int32 BindRetryCount = 0;

	/** Test 用的两个角色只请求一次（重新 Construct 时不重复请求） */
	bool bTestCharactersRequested = false;

	/** 下一次重建后要选中的角色；无效 Tag 表示不指定 */
	FGameplayTag PendingSelectedCharacterTag;

	/**
	 * 当前选中的角色，用于重建后恢复选中状态。
	 * 这里用 Tag 而不是 SlotIndex：槽位是 Server 分配的，客户端未必立刻知道。
	 */
	FGameplayTag CurrentSelectedCharacterTag;

	void CharacterSelected(UObject* SelectedUObject);
};
