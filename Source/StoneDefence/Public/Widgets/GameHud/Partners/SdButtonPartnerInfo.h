// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/GeniuxyCommonMacro.h"
#include "Widgets/Components/Button/GeCommonButtonSelection.h"
#include "Widgets/Components/ListEntryDataObjects/GeListEntryDataObjectBase.h"
#include "SdButtonPartnerInfo.generated.h"

class ASdCharacterHeroBase;

UCLASS()
class STONEDEFENCE_API UPartnerInfo : public UGeListEntryDataObjectBase
{
	GENERATED_BODY()

public:
	/**
	 * 这个类是 ASdPlayerStateGameBase 的"复制子对象"（AddReplicatedSubObject），
	 * 必须返回 true。
	 *
	 * UObject 默认返回 false，一旦被当成可复制对象处理，就会在
	 * FNetGUIDCache::IsDynamicObject 里的 check(Object->IsSupportedForNetworking())
	 * 直接断言挂掉（AssignNewNetGUID_Server 调用链上）。
	 */
	virtual bool IsSupportedForNetworking() const override { return true; }

	/**
	 * 子对象自身的属性也要走复制。
	 * 客户端上的对象是动态创建出来的空壳，不复制的属性会停在默认值
	 * （Tag 无效、SlotIndex 0、名字空、图标空）。
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	FGameplayTag CharacterTag;

	UPROPERTY(Replicated)
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(Replicated)
	FString CharacterName;

	UPROPERTY(Replicated)
	UTexture2D* CharacterIcon = nullptr;

public:
	DATA_ACCESSOR(FGameplayTag, CharacterTag)
	DATA_ACCESSOR(int, SlotIndex)
	DATA_ACCESSOR(FString, CharacterName)
	DATA_ACCESSOR(UTexture2D*, CharacterIcon)
};

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdButtonPartnerInfo : public UGeCommonButtonSelection
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonLazyImage* CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CharacterName;
	/********************/

	UPROPERTY()
	UPartnerInfo* PartnerInfoData;
};
