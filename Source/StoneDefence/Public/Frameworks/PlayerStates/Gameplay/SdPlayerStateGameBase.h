// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Frameworks/PlayerStates/SdPlayerStateBase.h"
#include "SdPlayerStateGameBase.generated.h"

class UPartnerInfo;
class UPA_CharacterDefinition;

/** PartnerInfos 发生变化时广播 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPartnerInfosChanged);

/**
 * 游戏局内中的 PlayerState。
 *
 * 除了基类的东西之外，这里额外记录玩家的"角色队伍"（可以切换的队友列表）。
 *
 * PartnerInfos 是**服务端权威**数据：
 * USdWidgetPartnerList 跑在客户端，所以它不直接改数组，而是调 RequestAddPartnerInfo()，
 * 由这里判断：
 *   - 有 Authority（Standalone / ListenServer 的 Host、DedicatedServer）-> 直接改；
 *   - 纯客户端 -> 转成 Server RPC，在服务端改，再复制回各客户端。
 *
 * 槽位一律由 Server 分配（除非显式指定）：
 * 客户端的 PartnerInfos 是复制来的，会滞后，自己算 "当前数量 + 1" 会撞车。
 *
 * UPartnerInfo 是运行时 New 出来的非 Actor UObject，需要注册成复制子对象才能复制，
 * 详见构造函数里的 bReplicateUsingRegisteredSubObjectList。
 */
UCLASS()
class STONEDEFENCE_API ASdPlayerStateGameBase : public ASdPlayerStateBase
{
	GENERATED_BODY()

public:
	ASdPlayerStateGameBase();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * PartnerInfos 变化通知。监听方（如 USdWidgetPartnerList）绑上之后
	 * 直接用 GetPartnerInfos() 重建 UI 即可。
	 *
	 * 两边都会触发：
	 *   - 客户端：由 OnRep_PartnerInfos 触发；
	 *   - Server（Standalone / ListenServer Host / DS）：服务端收不到自己的 OnRep，
	 *     所以在 AddPartnerInfo / ClearPartnerInfos 里手动广播。
	 */
	UPROPERTY(BlueprintAssignable, Category="Partner")
	FOnPartnerInfosChanged OnPartnerInfosChanged;

	/**
	 * 请求把一个队友加进队伍。
	 *
	 * @param InSlotIndex 想放的槽位；传 INDEX_NONE 表示由 Server 自动取下一个空槽位。
	 *                    客户端不要自己算槽位，交给 Server。
	 */
	void RequestAddPartnerInfo(FGameplayTag InCharacterTag, int32 InSlotIndex = INDEX_NONE);

	/** 清空队伍信息（重开 / 切场景时用，只在 Server 上有效） */
	void ClearPartnerInfos();

	/** 按槽位找队友，找不到返回 nullptr */
	UPartnerInfo* GetPartnerInfoBySlotIndex(int32 InSlotIndex) const;

	/** 按角色 Tag 找队友，找不到返回 nullptr */
	UPartnerInfo* FindPartnerInfoByCharacterTag(const FGameplayTag& InCharacterTag) const;

	FORCEINLINE const TArray<TObjectPtr<UPartnerInfo>>& GetPartnerInfos() const { return PartnerInfos; }
	FORCEINLINE int32 GetPartnerCount() const { return PartnerInfos.Num(); }

protected:
	/** 客户端收到 PartnerInfos 复制时触发 */
	UFUNCTION()
	void OnRep_PartnerInfos();

	/** 客户端 -> Server 的通道 */
	UFUNCTION(Server, Reliable)
	void Server_RequestAddPartnerInfo(FGameplayTag InCharacterTag, int32 InSlotIndex);

	/** 取下一个空槽位（当前最大槽位 + 1，最小是 1 —— 0 留给 EAbilityInputID::None） */
	int32 GetNextSlotIndex() const;

	/** 真正往数组里写。同一个 SlotIndex 只保留一份，重复调用会覆盖。只在 Server 上有效 */
	void AddPartnerInfo(UPartnerInfo* InPartnerInfo);

	/**
	 * 在 Server 上按 Tag 组装一个 UPartnerInfo 并写进数组。
	 * 资源还没加载完时会异步加载，加载好后再补写。
	 */
	void AddPartnerInfoFromTag(const FGameplayTag& InCharacterTag, int32 InSlotIndex);

	/**
	 * 角色队伍信息。服务端权威，只在 Server 上被修改。
	 * 注意：SlotIndex 不保证和数组下标一致，一律用 GetPartnerInfoBySlotIndex 查。
	 */
	UPROPERTY(ReplicatedUsing = OnRep_PartnerInfos)
	TArray<TObjectPtr<UPartnerInfo>> PartnerInfos;
};
