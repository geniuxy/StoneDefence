// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeClimbProbe.generated.h"

class UWorld;
class AActor;
class ACharacter;

/**
 * 攀爬探测的「输出」：够得着的那个 edge。
 *
 * 为什么放在 GeniuxyCommon：
 * - 探测实现（GeClimb::ProbeClimb）要被「角色的输入门控」和「GA_Climb」两处调用，
 *   而这两处分属不同模块（StoneDefence / GeniuxyGAS），只有 Common 是它们共同的底座；
 * - 探测本身只是一组世界几何查询，不依赖角色类、动画类或 GAS，放底层最合适。
 */
USTRUCT()
struct GENIUXYCOMMON_API FGeClimbLedgeInfo
{
	GENERATED_BODY()

	/** 手抓握点（edge 顶面，稍微往外偏一点，避免 warp 目标埋在墙皮里） */
	UPROPERTY()
	FVector LedgeLocation = FVector::ZeroVector;

	/** 墙面法线，指向「角色所在的那一侧」（引擎约定：命中法线朝查询起点） */
	UPROPERTY()
	FVector WallNormal = FVector::ZeroVector;

	/** 攀爬时角色要面向的方向：-WallNormal 的水平分量（即正对墙） */
	UPROPERTY()
	FVector ClimbForward = FVector::ForwardVector;

	bool IsValid() const { return !LedgeLocation.IsNearlyZero(); }

	void Reset() { *this = FGeClimbLedgeInfo(); }
};

/**
 * 攀爬探测的「输入」：调用方把自己身上的几何信息填进来。
 * 由 GeClimb::MakeProbeInput 从 ACharacter 生成，一般不用手填。
 */
USTRUCT()
struct GENIUXYCOMMON_API FGeClimbProbeInput
{
	GENERATED_BODY()

	/** 胶囊半径（cm） */
	UPROPERTY()
	float CapsuleRadius = 34.f;

	/** 胶囊半高（cm） */
	UPROPERTY()
	float CapsuleHalfHeight = 88.f;

	/** 胶囊中心的世界坐标（= 角色 ActorLocation） */
	UPROPERTY()
	FVector Origin = FVector::ZeroVector;

	/**
	 * 要往哪个方向探墙。
	 * 原地起攀用角色朝向；以后做「跳起抓边」时传速度的水平方向更自然。
	 */
	UPROPERTY()
	FVector Forward = FVector::ForwardVector;
};

/**
 * 攀爬探测的参数 —— 手感与几何阈值的唯一真源。
 *
 * 勾在 ASdCharacterBase 上（按武将调），GA_Climb 通过 IGeClimbProvider 读同一份，
 * **不要**在能力里再配第二份，否则会出现「门控说能爬、能力里探不到」这种最难查的分歧。
 *
 * 尺寸类参数一律用「相对胶囊的倍数」而不是绝对值：这样换成身高不同的武将时不用重新调一遍。
 */
USTRUCT(BlueprintType)
struct GENIUXYCOMMON_API FGeClimbProbeParams
{
	GENERATED_BODY()

	FGeClimbProbeParams()
	{
	}

	/** 前方探墙距离 = 胶囊半径 × 它 */
	UPROPERTY(EditAnywhere, Category = "Climb", meta = (ClampMin = "0.5", ClampMax = "6.0"))
	float ProbeDistanceRatio = 2.0f;

	/** 墙面法线 Z 超过它就算「能踩上去的坡」，不是要爬的墙 */
	UPROPERTY(EditAnywhere, Category = "Climb", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxWallNormalZ = 0.6f;

	/** 手够得到的高度 = 胶囊全高 × 它（1.0 ≈ 头顶）；从这个高度往下探 edge */
	UPROPERTY(EditAnywhere, Category = "Climb", meta = (ClampMin = "0.3", ClampMax = "2.0"))
	float ReachHeightRatio = 0.95f;

	/** 往下探 edge 的深度 = 胶囊半径 × 它 */
	UPROPERTY(EditAnywhere, Category = "Climb", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float EdgeProbeDepthRatio = 1.2f;

	/** edge 顶面的平整度下限（太斜的不是能抓/能站的边） */
	UPROPERTY(EditAnywhere, Category = "Climb", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinEdgeNormalZ = 0.7f;

	/** edge 上方给胶囊留的余量（cm）：放不下就说明抓着也上不去 */
	UPROPERTY(EditAnywhere, Category = "Climb", meta = (ClampMin = "0.0"))
	float StandClearance = 2.f;
};

namespace GeClimb
{
	/**
	 * 从角色身上取胶囊尺寸 / 位置 / 朝向，拼成探测输入。
	 * 抽出来是为了让「角色门控」和「GA_Climb」不用各写一遍这几行（少一份不一致的来源）。
	 */
	GENIUXYCOMMON_API FGeClimbProbeInput MakeProbeInput(const ACharacter& Character);

	/**
	 * 攀爬探测的唯一实现：能爬 → 填 OutInfo 并返回 true。
	 *
	 * 三步：
	 *   ① 前方 sweep 探墙（排除能踩上去的斜坡）；
	 *   ② 从「手够得到的高度」往墙内下方探 edge 的顶面；
	 *   ③ 检查 edge 上方放不放得下胶囊（放不下 = 抓着也上不去）。
	 *
	 * @param World        用来发查询的世界，调用方保证有效
	 * @param Input        胶囊几何 + 探测方向（见 MakeProbeInput）
	 * @param Params       阈值，见 FGeClimbProbeParams
	 * @param IgnoreActor  查询要忽略的 Actor（一般传角色自己）
	 * @param OutInfo      成功时写入 edge 数据
	 */
	GENIUXYCOMMON_API bool ProbeClimb(
		UWorld* World,
		const FGeClimbProbeInput& Input,
		const FGeClimbProbeParams& Params,
		const AActor* IgnoreActor,
		FGeClimbLedgeInfo& OutInfo);
}
