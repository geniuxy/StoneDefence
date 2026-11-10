// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/Instances/GeAnimInstanceBase.h"
#include "SdAnimInstanceCharacterBase.generated.h"

class ASdCharacterBase;
class UCharacterMovementComponent;

/**
 * 项目里所有「角色」AnimInstance 的基类（主实例这一条线）。
 *
 * 注意：速度、是否腾空、战斗状态这些通用量已经在 UGeAnimInstanceBase 里缓存好了
 * （GetSpeed / GetIsInAir / GetIsInCombat ...），这里只补项目特有、且「主实例」才需要的东西。
 *
 * 移动模式那组 getter（GetLocomotionMode / GetIsGrounded / GetIsFlying / GetIsSwimming /
 * GetIsClimbing / GetIsFlyingFast）**不在本类这条线上**：它们归 UGeAnimInstanceLocoBase，
 * 因为按模式换姿态的是 layer ABP。主 ABP 如果要用，需要在主实例自己缓存一份（自己读
 * UGeLocomotionComponent），不要反过来把这组数据塞回 UGeAnimInstanceBase。
 *
 * layer 实例（Fly/Swim/Climb）不是这个类，它们走 UGeAnimInstanceLocoBase 那条线，
 * 所以不要把「只有主实例才该有的逻辑」塞进 UGeAnimInstanceBase。
 */
UCLASS()
class STONEDEFENCE_API USdAnimInstanceCharacterBase : public UGeAnimInstanceBase
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	/**
	 * 游戏线程：读 Actor / MovementComponent 并缓存到本实例。
	 * 动画工作线程只读缓存，见 NativeThreadSafeUpdateAnimation。
	 */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	ASdCharacterBase* OwnerCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;
};
