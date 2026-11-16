// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GeAnimationSetup.generated.h"

class UAnimInstance;

/**
 * 每个角色的「动画配置」数据资产。加一个武将 = 加一个这个资产，而不是加一个 ABP。
 *
 * 配置入口：UPA_CharacterDefinition::AnimationSetup（和蒙太奇资产 UCharacterAnimationSet
 * 同一个入口，都在主数据资产里），运行时由角色推给 UGeAnimationComponent 持有。
 * 不要再在 BP 的组件默认值里给每个武将手挂一份 —— 那样配置就散在两个地方了。
 *
 * 它管两件事：
 * 1. LocomotionLayerClasses —— 运行时用 LinkAnimClassLayers 绑定哪些类；
 *    留空则回退到主 AnimInstance 类默认值里的 DefaultLocomotionLayerClasses。
 * 2. bCanFootIK —— 这个角色要不要脚部 IK。
 *
 * 为什么「选哪个类」必须放在数据里而不能放在 ABP 里：
 * Linked Anim Graph 节点的 Instance Class 是编译期固化的，子 ABP 覆盖不了；
 * 而 LinkAnimClassLayers 是运行时决定的，所以才能做到「主 ABP 一份、角色换数据」。
 *
 * 动画资产本身**不**放这里：各角色的差异用「层 ABP 里的 ParentAssetOverrides」
 * 覆盖父 ABP 的 Sequence / BlendSpace —— 两者都继承自 UAnimationAsset，都能覆写。
 */
UCLASS(BlueprintType)
class GENIUXYANIMATION_API UGeAnimationSetup : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * 移动层的实现类。留空则回退到 AnimInstance 类默认值里的 DefaultLocomotionLayerClasses。
	 * 一个类可以实现多个层（推荐：ABP_GeLoco 一次实现 4 层）。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Locomotion")
	TArray<TSoftClassPtr<UAnimInstance>> LocomotionLayerClasses;

	/** 同步加载并返回层类；失败或为空返回空数组 */
	TArray<TSubclassOf<UAnimInstance>> LoadLocomotionLayerClasses() const;

	/**
	 * 这个角色要不要脚部 IK。
	 *
	 * 数据流：本资产由 UPA_CharacterDefinition::AnimationSetup 指定 → 角色拿到 PA 后
	 *        调 UGeAnimationComponent::SetAnimationSetup 推过去 → 组件把它同步成
	 *        UGeAnimationComponent::bFootIKEnabled → 主实例每帧从组件读
	 *        （UGeAnimInstanceBase::NativeUpdateAnimation 末尾；移动模式另从
	 *        UGeLocomotionComponent 读，两者都在游戏线程缓存后才给工作线程用）。
	 *
	 * 不要在 ABP 的 Class Defaults 里配 bCanFootIK：那里配的只有该 ABP 自己的实例读得到，
	 * 而 FootIK 的 ControlRig 在主 ABP 里、读的是主实例那一份；主 ABP 又只有一份，
	 * 物理上没法按角色区分。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Foot IK")
	bool bCanFootIK = true;
};
