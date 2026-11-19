// Fill out your copyright notice in the Description page of Project Settings.


#include "Climb/GeClimbProbe.h"

#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

namespace
{
	/** 地面/墙体查询都走这个通道；如果以后墙体单独配了通道，只改这里 */
	constexpr ECollisionChannel ClimbProbeChannel = ECC_Visibility;
}

FGeClimbProbeInput GeClimb::MakeProbeInput(const ACharacter& Character)
{
	FGeClimbProbeInput Input;

	if (const UCapsuleComponent* Capsule = Character.GetCapsuleComponent())
	{
		Input.CapsuleRadius = Capsule->GetScaledCapsuleRadius();
		Input.CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	}

	Input.Origin = Character.GetActorLocation();
	Input.Forward = Character.GetActorForwardVector();
	return Input;
}

bool GeClimb::ProbeClimb(
	UWorld* World,
	const FGeClimbProbeInput& Input,
	const FGeClimbProbeParams& Params,
	const AActor* IgnoreActor,
	FGeClimbLedgeInfo& OutInfo)
{
	OutInfo.Reset();

	if (!World || Input.CapsuleRadius <= 0.f || Input.CapsuleHalfHeight <= 0.f)
	{
		return false;
	}

	// 探测方向只取水平分量：抬头看天的时候不该往天上探墙
	const FVector Forward = Input.Forward.GetSafeNormal2D();
	if (Forward.IsNearlyZero())
	{
		return false;
	}

	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(GeClimbProbe), /*bTraceComplex=*/false, IgnoreActor);
	const float FeetZ = Input.Origin.Z - Input.CapsuleHalfHeight;

	// ① 前方探墙：用「半高胶囊」扫，免得头顶先蹭到墙沿就判定为撞墙
	const FCollisionShape WallProbeShape =
		FCollisionShape::MakeCapsule(Input.CapsuleRadius, Input.CapsuleHalfHeight * 0.5f);
	const FVector WallEnd = Input.Origin + Forward * (Input.CapsuleRadius * Params.ProbeDistanceRatio);

	FHitResult WallHit;
	if (!World->SweepSingleByChannel(
		WallHit, Input.Origin, WallEnd, FQuat::Identity, ClimbProbeChannel, WallProbeShape, QueryParams))
	{
		return false;
	}

	// 法线朝上太多的算斜坡（能走上去），不归攀爬管
	if (WallHit.ImpactNormal.Z > Params.MaxWallNormalZ)
	{
		return false;
	}

	// ② 从「手够得到的高度」往墙内下方探 edge 的顶面。
	//    起点要压进墙内一点点：沿着墙面往下打只会擦着墙皮，打不到顶面。
	const float ReachHeight = Input.CapsuleHalfHeight * 2.f * Params.ReachHeightRatio;
	const float EdgeProbeDepth = Input.CapsuleRadius * Params.EdgeProbeDepthRatio;

	const FVector EdgeStart =
		FVector(WallHit.ImpactPoint.X, WallHit.ImpactPoint.Y, FeetZ + ReachHeight + EdgeProbeDepth)
		- WallHit.ImpactNormal * (Input.CapsuleRadius * 0.5f);

	FHitResult EdgeHit;
	if (!World->LineTraceSingleByChannel(
		EdgeHit, EdgeStart, EdgeStart - FVector(0.f, 0.f, EdgeProbeDepth), ClimbProbeChannel, QueryParams))
	{
		return false;
	}

	// 打到的必须是「顶面」（法线朝上），否则打的是斜面 / 尖角
	if (EdgeHit.ImpactNormal.Z < Params.MinEdgeNormalZ)
	{
		return false;
	}

	// ③ edge 上方得放得下胶囊，否则抓着也站不上去
	// const FVector StandCenter =
	// 	EdgeHit.ImpactPoint + FVector(0.f, 0.f, Input.CapsuleHalfHeight + Params.StandClearance);
	// if (World->OverlapAnyTestByChannel(
	// 	StandCenter, FQuat::Identity, ClimbProbeChannel,
	// 	FCollisionShape::MakeCapsule(Input.CapsuleRadius * 0.9f, Input.CapsuleHalfHeight * 0.9f), QueryParams))
	// {
	// 	return false;
	// }

	// 命中法线朝查询起点（= 朝角色这一侧），所以 +WallNormal 是「朝角色挪」，-WallNormal 是「朝墙里挪」
	OutInfo.WallNormal = WallHit.ImpactNormal;
	OutInfo.ClimbForward = FVector::VectorPlaneProject(-WallHit.ImpactNormal, FVector::UpVector).GetSafeNormal();
	OutInfo.LedgeLocation = EdgeHit.ImpactPoint + WallHit.ImpactNormal * (Input.CapsuleRadius * 0.2f);
	return true;
}
