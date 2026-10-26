// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SdAnimationSetup.generated.h"

class UAnimInstance;
class UGeLocomotionAnimSet;

/**
 * 每个武将的「动画配置」。加一个武将 = 加一个这个资产，而不是加一个 ABP。
 *
 * 它管两件事：
 * 1. LocomotionLayerClasses —— 运行时用 LinkAnimClassLayers 绑定哪些类；
 *    留空就用主 ABP 上 UAnimInstance 类默认值里的那一组（即插件默认层）。
 * 2. LocomotionAnimSet —— 这个武将用哪套飞行/游泳/攀爬动画资产。
 *
 * 之所以把「选哪个类」放在数据里而不是 ABP 里：
 * Linked Anim Graph 节点的 Instance Class 是编译期固化的，子 ABP 覆盖不了；
 * 而 LinkAnimClassLayers 是运行时决定的，所以才能做到「主 ABP 一份、武将换数据」。
 */
UCLASS(BlueprintType)
class STONEDEFENCE_API USdAnimationSetup : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * 移动层的实现类。留空则回退到 AnimInstance 类默认值里的 DefaultLocomotionLayerClasses。
	 * 一个类可以实现多个层（推荐：插件的 ABP_GeLoco 一次实现 4 层）。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Locomotion")
	TArray<TSoftClassPtr<UAnimInstance>> LocomotionLayerClasses;

	/** 这个武将的移动动画集合（飞行/游泳/攀爬的混合空间等） */
	UPROPERTY(EditDefaultsOnly, Category = "Locomotion")
	TSoftObjectPtr<UGeLocomotionAnimSet> LocomotionAnimSet;

	/** 同步加载并返回层类；失败或为空返回空数组 */
	TArray<TSubclassOf<UAnimInstance>> LoadLocomotionLayerClasses() const;

	/** 同步加载动画集合 */
	UGeLocomotionAnimSet* LoadLocomotionAnimSet() const;
};
