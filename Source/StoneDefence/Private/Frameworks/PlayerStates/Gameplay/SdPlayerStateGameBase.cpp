// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/PlayerStates/Gameplay/SdPlayerStateGameBase.h"

#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "Frameworks/SdAssetManager.h"
#include "GeniuxyDebugHelper.h"
#include "Net/UnrealNetwork.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemCharacter.h"
#include "Widgets/GameHud/Partners/SdButtonPartnerInfo.h"

namespace
{
	/** 用角色定义组一个 UPartnerInfo。Outer 给 PlayerState，才能注册成它的复制子对象 */
	UPartnerInfo* MakePartnerInfo(
		UObject* InOuter,
		const UPA_CharacterDefinition* InDefinition,
		const FGameplayTag& InCharacterTag,
		int32 InSlotIndex)
	{
		if (!InOuter || !InDefinition) return nullptr;

		UPartnerInfo* PartnerInfo = NewObject<UPartnerInfo>(InOuter);
		PartnerInfo->SetCharacterTag(InCharacterTag);
		PartnerInfo->SetCharacterName(InDefinition->GetCharacterDisplayName());
		PartnerInfo->SetCharacterIcon(InDefinition->LoadIcon());
		PartnerInfo->SetSlotIndex(InSlotIndex);
		return PartnerInfo;
	}
}

ASdPlayerStateGameBase::ASdPlayerStateGameBase()
{
	// UPartnerInfo 是运行时 New 出来的非 Actor UObject。
	// 默认这种对象不会被 NetGUID 支持，客户端会收到：
	//   LogNetPackageMap: Warning: FNetGUIDCache::SupportsObject: PartnerInfo ... NOT Supported.
	// 打开"注册式子对象复制"再配合 AddReplicatedSubObject，才能把它复制过去。
	bReplicateUsingRegisteredSubObjectList = true;
}

void ASdPlayerStateGameBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASdPlayerStateGameBase, PartnerInfos);
}

/** 只有客户端会走到这里（服务端不会收到自己的 OnRep） */
void ASdPlayerStateGameBase::OnRep_PartnerInfos()
{
	OnPartnerInfosChanged.Broadcast();
}

int32 ASdPlayerStateGameBase::GetNextSlotIndex() const
{
	int32 MaxSlotIndex = 0;
	for (const TObjectPtr<UPartnerInfo>& Info : PartnerInfos)
	{
		if (Info)
		{
			MaxSlotIndex = FMath::Max(MaxSlotIndex, Info->GetSlotIndex());
		}
	}

	// 槽位从 1 开始（0 留给 EAbilityInputID::None）
	return MaxSlotIndex + 1;
}

void ASdPlayerStateGameBase::RequestAddPartnerInfo(FGameplayTag InCharacterTag, int32 InSlotIndex)
{
	if (!InCharacterTag.IsValid()) return;

	// 服务端（含 Standalone / ListenServer 的 Host）直接改；
	// 纯客户端转成 Server RPC，保证 PartnerInfos 只在 Server 上被修改
	if (HasAuthority())
	{
		AddPartnerInfoFromTag(InCharacterTag, InSlotIndex);
	}
	else
	{
		Server_RequestAddPartnerInfo(InCharacterTag, InSlotIndex);
	}
}

void ASdPlayerStateGameBase::Server_RequestAddPartnerInfo_Implementation(
	FGameplayTag InCharacterTag, int32 InSlotIndex)
{
	AddPartnerInfoFromTag(InCharacterTag, InSlotIndex);
}

void ASdPlayerStateGameBase::AddPartnerInfoFromTag(const FGameplayTag& InCharacterTag, int32 InSlotIndex)
{
	// 兜底：这个函数只允许在 Server 上跑
	if (!HasAuthority() || !InCharacterTag.IsValid()) return;

	// 槽位由 Server 分配：客户端的 PartnerInfos 是复制来的，会滞后，
	// 两端各自算 "当前数量 + 1" 必然撞车（两个队友都会被塞进同一个槽位）。
	if (InSlotIndex == INDEX_NONE)
	{
		InSlotIndex = GetNextSlotIndex();
	}

	UPA_CharacterDefinition* Definition =
		USdAssetManager::Get().GetCharacterDefinition(InCharacterTag.GetTagName());

	if (!Definition)
	{
		// 资源还没加载完：加载好之后在服务端补写
		USdGISubsystemCharacter* Subsystem = USdGISubsystemCharacter::Get(this);
		if (!Subsystem) return;

		Subsystem->LoadCharacter(
			InCharacterTag,
			FOnCharacterDefinitionLoaded::CreateWeakLambda(
				this,
				[this, InCharacterTag, InSlotIndex](const UPA_CharacterDefinition* LoadedDefinition)
				{
					if (!HasAuthority() || !LoadedDefinition) return;

					if (UPartnerInfo* PartnerInfo =
						MakePartnerInfo(this, LoadedDefinition, InCharacterTag, InSlotIndex))
					{
						AddPartnerInfo(PartnerInfo);
					}
				}
			)
		);
		return;
	}

	if (UPartnerInfo* PartnerInfo = MakePartnerInfo(this, Definition, InCharacterTag, InSlotIndex))
	{
		AddPartnerInfo(PartnerInfo);
	}
}

void ASdPlayerStateGameBase::AddPartnerInfo(UPartnerInfo* InPartnerInfo)
{
	// PartnerInfos 只在 Server 上被修改
	if (!HasAuthority())
	{
		Debug::Print(TEXT("[SdPlayerStateGameBase] AddPartnerInfo 只能在 Server 上调用，已忽略"));
		return;
	}

	if (!InPartnerInfo) return;

	const int32 InSlotIndex = InPartnerInfo->GetSlotIndex();

	// 同一个槽位只留一份
	bool bReplaced = false;
	for (TObjectPtr<UPartnerInfo>& Existing : PartnerInfos)
	{
		if (Existing && Existing->GetSlotIndex() == InSlotIndex)
		{
			// 先把旧的注销掉，客户端的旧副本会被自动清掉
			RemoveReplicatedSubObject(Existing);
			Existing = InPartnerInfo;
			bReplaced = true;
			break;
		}
	}

	if (!bReplaced)
	{
		PartnerInfos.Add(InPartnerInfo);
	}

	// 注册成复制子对象 —— 不做这一步客户端根本收不到
	AddReplicatedSubObject(InPartnerInfo);

	Debug::Print(FString::Printf(
		TEXT("[SdPlayerStateGameBase] 记录队友 Tag=%s Slot=%d，当前共 %d 个"),
		*InPartnerInfo->GetCharacterTag().ToString(), InSlotIndex, PartnerInfos.Num()
	));

	// Server 收不到自己的 OnRep，手动通知一次，保证 Host 的 UI 也能刷新
	OnPartnerInfosChanged.Broadcast();
}

void ASdPlayerStateGameBase::ClearPartnerInfos()
{
	// 同上：只在 Server 上改
	if (!HasAuthority()) return;

	for (const TObjectPtr<UPartnerInfo>& Info : PartnerInfos)
	{
		if (Info)
		{
			RemoveReplicatedSubObject(Info);
		}
	}

	PartnerInfos.Reset();

	// Server 收不到自己的 OnRep，手动通知一次
	OnPartnerInfosChanged.Broadcast();
}

UPartnerInfo* ASdPlayerStateGameBase::GetPartnerInfoBySlotIndex(int32 InSlotIndex) const
{
	for (const TObjectPtr<UPartnerInfo>& Info : PartnerInfos)
	{
		if (Info && Info->GetSlotIndex() == InSlotIndex)
		{
			return Info.Get();
		}
	}

	return nullptr;
}

UPartnerInfo* ASdPlayerStateGameBase::FindPartnerInfoByCharacterTag(const FGameplayTag& InCharacterTag) const
{
	for (const TObjectPtr<UPartnerInfo>& Info : PartnerInfos)
	{
		if (Info && Info->GetCharacterTag() == InCharacterTag)
		{
			return Info.Get();
		}
	}

	return nullptr;
}
