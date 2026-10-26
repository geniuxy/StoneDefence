// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdAnimInstanceCharacterBase.h"
#include "SdAnimInstanceHeroBase.generated.h"

class ASdCharacterHeroBase;
class UGeAnimationSetup;

/**
 * 所有武将共用的 AnimInstance 类。**不要**再为单个武将派生新的 AnimInstance 类或新的主 ABP。
 *
 * 它是怎么做到「主 ABP 一份、武将换数据」的：
 *   主 ABP（ABP_SdHeroBase）的 AnimGraph 里放的是 Linked **Anim Layer** 节点，
 *   这些节点只声明「我要 ALI_GeLocomotion 的 FlyLocomotion 层」，
 *   具体由哪个类实现，是在这里用 LinkAnimClassLayers() 在**运行时**指定的。
 *
 * 对比一下为什么旧的 Linked Anim Graph 不行：
 *   FAnimNode_LinkedAnimGraph::InstanceClass 是普通节点属性，
 *   而子 ABP 能覆盖的只有 ParentAssetOverrides 里的 UAnimationAsset，
 *   所以「在主 ABP 的 ChildABP 里改掉嵌套子 ABP 的类」永远做不到。
 *
 * 武将之间的差异全部落在 UGeAnimationSetup（插件里的数据资产）上：
 *   - LocomotionLayerClasses ：选哪套层实现（默认用插件的 ABP_GeLoco）
 *   - bCanFootIK             ：这个角色要不要脚部 IK
 *
 * 动画资产本身不走数据：各角色的差异用「层 ABP 的 ParentAssetOverrides」覆盖
 * 父 ABP 里的 Sequence / BlendSpace（两者都是 UAnimationAsset，都能覆写）。
 */
UCLASS()
class STONEDEFENCE_API USdAnimInstanceHeroBase : public USdAnimInstanceCharacterBase
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	/**
	 * 按当前 UGeAnimationSetup 重新绑定移动层。
	 *
	 * 什么时候会被调用：
	 * - NativeInitializeAnimation 时（如果配置已经就绪）
	 * - UGeLocomotionComponent 上的 UGeAnimationSetup 被换掉时，由组件广播事件触发
	 *
	 * 不要每帧调：LinkAnimClassLayers 会重建 linked instance。
	 */
	UFUNCTION(BlueprintCallable, Category = "Geniuxy|Locomotion")
	void RefreshLocomotionLayers();

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetNeedFaceSculpt() const { return bNeedFaceSculpt; }

protected:
	virtual void CacheDerivedState() override;

	/**
	 * 兜底的层实现类。
	 *
	 * 在 ABP_SdHeroBase 的 Class Defaults 里填一次即可（通常就填插件的 ABP_GeLoco）。
	 * 当某个武将的 UGeAnimationSetup 没有指定 LocomotionLayerClasses 时用这一组。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Locomotion")
	TArray<TSoftClassPtr<UAnimInstance>> DefaultLocomotionLayerClasses;

	/** 当前生效的层实现类，用于判重，避免重复 Link */
	UPROPERTY(Transient)
	TArray<TSubclassOf<UAnimInstance>> AppliedLayerClasses;

	UPROPERTY()
	ASdCharacterHeroBase* OwnerHero;

	/**********************************************************************/
	/*                          Figure Type Size                          */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE int32 GetLegSize() const { return LegSize; }
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE int32 GetWaistSize() const { return WaistSize; }
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE int32 GetArmSize() const { return ArmSize; }
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FORCEINLINE int32 GetHeadSize() const { return HeadSize; }
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
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
