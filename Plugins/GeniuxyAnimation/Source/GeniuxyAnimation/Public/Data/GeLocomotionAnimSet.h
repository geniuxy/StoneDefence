// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GeLocomotionAnimSet.generated.h"

class UBlendSpace;
class UAnimSequenceBase;

/**
 * 一套「移动动画资产」。
 *
 * 设计目的：让 ABP 只有一份（插件里的 ABP_GeLoco），不同武将的差异只体现为
 * 指向不同的 GeLocomotionAnimSet。层 ABP 里 BlendSpace / Sequence 的引脚不要写死资产，
 * 而是绑到 AnimInstance 上的 GetFlyBlendSpace() / GetSwimBlendSpace() ... 这些取值函数。
 *
 * 这样加一个武将 = 加一个 DataAsset，而不是加一个 ABP。
 *
 * 线程安全约定：
 * - 软引用只在 PreloadAll()（游戏线程，角色初始化时）解析一次，结果缓存在下面的 TObjectPtr 里；
 * - 所有 Get* 函数只读缓存，不加载任何东西，因此可以放心地在动画工作线程里调用
 *   （它们都带 BlueprintThreadSafe）。
 */
UCLASS(BlueprintType)
class GENIUXYANIMATION_API UGeLocomotionAnimSet : public UDataAsset
{
	GENERATED_BODY()

public:
	/**********************************************************************/
	/*                                Fly                                 */
	/**********************************************************************/
	/** 飞行移动混合空间（X/Y 接 GetFlySpeed 的 X/Y） */
	UPROPERTY(EditDefaultsOnly, Category = "Fly")
	TSoftObjectPtr<UBlendSpace> FlyBlendSpace;

	/** 起飞过渡（可选，接 SequencePlayer 的 Sequence） */
	UPROPERTY(EditDefaultsOnly, Category = "Fly")
	TSoftObjectPtr<UAnimSequenceBase> FlyStartSequence;

	/** 降落过渡（可选） */
	UPROPERTY(EditDefaultsOnly, Category = "Fly")
	TSoftObjectPtr<UAnimSequenceBase> FlyStopSequence;

	/**********************************************************************/
	/*                                Swim                                */
	/**********************************************************************/
	/** 游泳移动混合空间 */
	UPROPERTY(EditDefaultsOnly, Category = "Swim")
	TSoftObjectPtr<UBlendSpace> SwimBlendSpace;

	/** 入水过渡（可选） */
	UPROPERTY(EditDefaultsOnly, Category = "Swim")
	TSoftObjectPtr<UAnimSequenceBase> SwimStartSequence;

	/**********************************************************************/
	/*                                Climb                               */
	/**********************************************************************/
	/** 攀爬移动混合空间（X/Y 接 GetClimbSpeed 的 X/Y） */
	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	TSoftObjectPtr<UBlendSpace> ClimbBlendSpace;

	/** 攀爬上/下（可选，没有就只走混合空间） */
	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	TSoftObjectPtr<UAnimSequenceBase> ClimbUpSequence;

	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	TSoftObjectPtr<UAnimSequenceBase> ClimbDownSequence;

	/**********************************************************************/
	/*                               Accessors                            */
	/**********************************************************************/
	/**
	 * 游戏线程调用：把上面所有软引用解析成硬引用并缓存。
	 * 必须在动画开始跑之前调（建议在角色 BeginPlay / 设置 AnimSet 时调）。
	 */
	void PreloadAll();

	/** 下面这些只读缓存，不加载，可在动画工作线程调用 */
	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion", meta = (BlueprintThreadSafe))
	UBlendSpace* GetFlyBlendSpace() const { return CachedFlyBlendSpace; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion", meta = (BlueprintThreadSafe))
	UAnimSequenceBase* GetFlyStartSequence() const { return CachedFlyStartSequence; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion", meta = (BlueprintThreadSafe))
	UAnimSequenceBase* GetFlyStopSequence() const { return CachedFlyStopSequence; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion", meta = (BlueprintThreadSafe))
	UBlendSpace* GetSwimBlendSpace() const { return CachedSwimBlendSpace; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion", meta = (BlueprintThreadSafe))
	UAnimSequenceBase* GetSwimStartSequence() const { return CachedSwimStartSequence; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion", meta = (BlueprintThreadSafe))
	UBlendSpace* GetClimbBlendSpace() const { return CachedClimbBlendSpace; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion", meta = (BlueprintThreadSafe))
	UAnimSequenceBase* GetClimbUpSequence() const { return CachedClimbUpSequence; }

	UFUNCTION(BlueprintPure, Category = "Geniuxy|Locomotion", meta = (BlueprintThreadSafe))
	UAnimSequenceBase* GetClimbDownSequence() const { return CachedClimbDownSequence; }

protected:
	/** 解析后的硬引用，由 PreloadAll 填充，防止资产被回收 */
	UPROPERTY(Transient)
	TObjectPtr<UBlendSpace> CachedFlyBlendSpace;

	UPROPERTY(Transient)
	TObjectPtr<UAnimSequenceBase> CachedFlyStartSequence;

	UPROPERTY(Transient)
	TObjectPtr<UAnimSequenceBase> CachedFlyStopSequence;

	UPROPERTY(Transient)
	TObjectPtr<UBlendSpace> CachedSwimBlendSpace;

	UPROPERTY(Transient)
	TObjectPtr<UAnimSequenceBase> CachedSwimStartSequence;

	UPROPERTY(Transient)
	TObjectPtr<UBlendSpace> CachedClimbBlendSpace;

	UPROPERTY(Transient)
	TObjectPtr<UAnimSequenceBase> CachedClimbUpSequence;

	UPROPERTY(Transient)
	TObjectPtr<UAnimSequenceBase> CachedClimbDownSequence;
};
