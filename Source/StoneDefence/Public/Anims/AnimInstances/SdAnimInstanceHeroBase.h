// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdAnimInstanceCharacterBase.h"
#include "SdAnimInstanceHeroBase.generated.h"

class ASdCharacterHeroBase;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdAnimInstanceHeroBase : public USdAnimInstanceCharacterBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	/**
	 * 游戏线程：从角色身上把捏脸数据拷进本实例的缓存。
	 * 动画工作线程只读缓存，绝不去读 Actor（见 NativeThreadSafeUpdateAnimation）。
	 */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetNeedFaceSculpt() const { return bNeedFaceSculpt; }

protected:
	UPROPERTY()
	ASdCharacterHeroBase* OwnerHero;

	/**********************************************************************/
	/*                          Figure Type Size                          */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetLegSize() const { return LegSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetWaistSize() const { return WaistSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetArmSize() const { return ArmSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetHeadSize() const { return HeadSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetChestSize() const { return ChestSize; }

protected:
	/**
	 * 下面这些值只在 NativeUpdateAnimation（游戏线程）里写。
	 * NativeThreadSafeUpdateAnimation 跑在动画工作线程上，只能读这里，
	 * 不能反过来去读 Actor / 角色的 TMap，否则是数据竞争。
	 */
	int32 LegSize = 0;
	int32 WaistSize = 0;
	int32 ArmSize = 0;
	int32 HeadSize = 1;
	int32 ChestSize = 1;

	bool bNeedFaceSculpt = false;
};
