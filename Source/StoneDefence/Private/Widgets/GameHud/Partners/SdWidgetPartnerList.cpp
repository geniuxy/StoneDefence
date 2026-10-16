// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameHud/Partners/SdWidgetPartnerList.h"

#include "CommonListView.h"
#include "Frameworks/PlayerStates/Gameplay/SdPlayerStateGameBase.h"
#include "GeniuxyDebugHelper.h"
#include "GeniuxyGASType.h"
#include "Tags/CharacterTags.h"
#include "TimerManager.h"
#include "Widgets/GameHud/Partners/SdButtonPartnerInfo.h"

namespace
{
	/** PlayerState 还没到位时的重试间隔 */
	constexpr float PlayerStateBindRetryInterval = 0.05f;

	/** 最多重试这么多次（约 5 秒）就放弃，避免无限轮询 */
	constexpr int32 PlayerStateBindMaxRetry = 100;
}

void USdWidgetPartnerList::AddCharacter(FGameplayTag InCharacterTag, bool bSelected, int32 InSlotIndex)
{
	if (!InCharacterTag.IsValid()) return;

	// PartnerInfos 是服务端权威数据，这里只负责"请求"。
	// 列表不在这里手动加，等数据回来（服务端直接广播 / 客户端 OnRep）之后统一重建。
	ASdPlayerStateGameBase* GamePlayerState = GetOwningPlayerState<ASdPlayerStateGameBase>();
	if (!GamePlayerState)
	{
		Debug::Print(TEXT("[SdWidgetPartnerList] PlayerState 还没到位，队伍信息未记录"));
		return;
	}

	if (bSelected)
	{
		PendingSelectedCharacterTag = InCharacterTag;
	}

	// InSlotIndex 为 INDEX_NONE 时由 Server 分配槽位。
	// 这里千万不要自己算 "GetPartnerCount() + 1"：客户端的数组是复制来的、会滞后，
	// 连续两次调用会拿到同一个值，结果第二个把第一个顶掉。
	GamePlayerState->RequestAddPartnerInfo(InCharacterTag, InSlotIndex);
}

void USdWidgetPartnerList::TryBindPlayerState()
{
	if (bPlayerStateBound) return;

	UWorld* World = GetWorld();
	if (!World)
	{
		Debug::Print(TEXT("[SdWidgetPartnerList] 拿不到 World，队伍列表无法绑定"));
		return;
	}

	ASdPlayerStateGameBase* GamePlayerState = GetOwningPlayerState<ASdPlayerStateGameBase>();

	if (!GamePlayerState)
	{
		// 还没复制到位：挂个重复定时器继续等
		if (++BindRetryCount > PlayerStateBindMaxRetry)
		{
			World->GetTimerManager().ClearTimer(BindPlayerStateTimerHandle);

			const APlayerState* CurrentPlayerState = GetOwningPlayerState();
			Debug::Print(FString::Printf(
				TEXT("[SdWidgetPartnerList] 等 ASdPlayerStateGameBase 超时，当前 PlayerState = %s"),
				CurrentPlayerState ? *CurrentPlayerState->GetClass()->GetName() : TEXT("NULL")
			));
			return;
		}

		World->GetTimerManager().SetTimer(
			BindPlayerStateTimerHandle,
			this,
			&ThisClass::TryBindPlayerState,
			PlayerStateBindRetryInterval,
			true
		);
		return;
	}

	// 绑定成功，停掉重试
	World->GetTimerManager().ClearTimer(BindPlayerStateTimerHandle);
	bPlayerStateBound = true;

	// PartnerInfos 一变就重建列表
	GamePlayerState->OnPartnerInfosChanged.AddDynamic(this, &ThisClass::HandlePartnerInfosChanged);

	// 数据可能在这个 widget 创建之前就已经复制过来了，先按当前状态建一次
	RebuildPartnerList();

	// Test
	// 槽位交给 Server 自动分配（1、2、3……），这里不要再传 EAbilityInputID 的值。
	// InputKey -> 队伍槽位 的换算由 UGA_SwitchCharacter 负责：
	// 键盘 1/2/3/4 绑的是 AbilityOne..AbilityFour（枚举值 3/4/5/6），
	// 换算出 1/2/3/4，正好对上这里的槽位。
	if (!bTestCharactersRequested)
	{
		bTestCharactersRequested = true;
		AddCharacter(CharacterTags::Sd_Character_Tubaki);
		AddCharacter(CharacterTags::Sd_Character_Cleric, false);
	}
}

void USdWidgetPartnerList::RebuildPartnerList()
{
	ASdPlayerStateGameBase* GamePlayerState = GetOwningPlayerState<ASdPlayerStateGameBase>();
	if (!GamePlayerState || !CharacterList || !GetWorld()) return;

	// ClearListItems 会把选中状态一起清掉，所以先决定重建后要选谁：
	// 优先用本次显式指定的角色，否则沿用上一次选中的角色
	FGameplayTag TagToSelect = PendingSelectedCharacterTag;
	if (!TagToSelect.IsValid())
	{
		TagToSelect = CurrentSelectedCharacterTag;
	}
	PendingSelectedCharacterTag = FGameplayTag();

	CharacterList->ClearListItems();

	bool bTagFound = false;

	for (const TObjectPtr<UPartnerInfo>& PartnerInfo : GamePlayerState->GetPartnerInfos())
	{
		if (!PartnerInfo) continue;

		// 直接拿 PlayerState 里那份复制下来的数据当 ListItem，不再本地 New
		CharacterList->AddItem(PartnerInfo.Get());

		if (TagToSelect.IsValid() && PartnerInfo->GetCharacterTag() == TagToSelect)
		{
			bTagFound = true;
		}
	}

	CurrentSelectedCharacterTag = bTagFound ? TagToSelect : FGameplayTag();

	if (bTagFound)
	{
		// 等 ListView 这一轮重建完再选中；用 Tag 重新查一次，
		// 避免中间又插进来一次重建导致拿到的 Item 已经不在列表里
		const FGameplayTag TagToSelectCopy = TagToSelect;
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateLambda([this, TagToSelectCopy]()
			{
				if (ASdPlayerStateGameBase* PS = GetOwningPlayerState<ASdPlayerStateGameBase>())
				{
					if (UPartnerInfo* TargetInfo = PS->FindPartnerInfoByCharacterTag(TagToSelectCopy))
					{
						CharacterList->SetSelectedItem(TargetInfo);
					}
				}
			})
		);
	}
}

void USdWidgetPartnerList::HandlePartnerInfosChanged()
{
	RebuildPartnerList();
}

void USdWidgetPartnerList::NativePreConstruct()
{
	Super::NativePreConstruct();

	CharacterList->SetScrollbarVisibility(ESlateVisibility::Collapsed);
}

void USdWidgetPartnerList::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterList->ClearListItems();
	PendingSelectedCharacterTag = FGameplayTag();
	CurrentSelectedCharacterTag = FGameplayTag();

	CharacterList->OnItemSelectionChanged().AddUObject(this, &ThisClass::CharacterSelected);

	// GameHud 是在 PlayerController::BeginPlay 里建的，客户端上那时 PlayerState 往往还没复制过来，
	// 所以这里要等到它到位再绑定
	bPlayerStateBound = false;
	BindRetryCount = 0;
	TryBindPlayerState();
}

void USdWidgetPartnerList::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BindPlayerStateTimerHandle);
	}

	if (ASdPlayerStateGameBase* GamePlayerState = GetOwningPlayerState<ASdPlayerStateGameBase>())
	{
		GamePlayerState->OnPartnerInfosChanged.RemoveDynamic(this, &ThisClass::HandlePartnerInfosChanged);
	}

	bPlayerStateBound = false;

	Super::NativeDestruct();
}

void USdWidgetPartnerList::CharacterSelected(UObject* SelectedUObject)
{
	// 取消选择时 SelectedUObject 是 nullptr，这时保留上一次记录的角色
	if (const UPartnerInfo* PartnerInfo = Cast<UPartnerInfo>(SelectedUObject))
	{
		CurrentSelectedCharacterTag = PartnerInfo->GetCharacterTag();
	}
}
