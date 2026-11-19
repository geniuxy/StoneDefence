// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Climb/GeClimbProbe.h"
#include "GeClimbProvider.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UGeClimbProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 攀爬探测参数的提供者 —— GAS 侧与角色侧之间的依赖倒置接口（照 IGeStateProvider 的套路）。
 *
 * 为什么需要它：
 * - 门控在角色上（ASdCharacterBase::CanStartClimb），参数的唯一真源也在角色上；
 * - GA_Climb 在 GeniuxyGAS 模块里，而 GeniuxyGAS **不依赖** StoneDefence，看不到角色类；
 * - 但两边必须用**同一份**参数，否则会出现「门控说能爬、能力里探不到」的分歧。
 *
 * 于是：角色实现本接口，能力只认识接口。GeniuxyGAS 已经依赖 GeniuxyCommon，不会破坏插件边界。
 *
 * 注意：这里**只暴露参数**，不暴露探测函数 —— 探测的唯一实现是 GeClimb::ProbeClimb，
 * 门控和能力各自去调它，不在实现方代理一层。
 */
class GENIUXYCOMMON_API IGeClimbProvider
{
	GENERATED_BODY()

public:
	/** 攀爬探测参数的唯一真源（实现方直接返回自己的成员，不做任何查找） */
	virtual const FGeClimbProbeParams& GetClimbProbeParams() const = 0;
};
