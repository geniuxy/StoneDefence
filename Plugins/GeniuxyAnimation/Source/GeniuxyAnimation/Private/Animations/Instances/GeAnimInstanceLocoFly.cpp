// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Instances/GeAnimInstanceLocoFly.h"

void UGeAnimInstanceLocoFly::CacheDerivedState()
{
	Super::CacheDerivedState();

	// 只用基类缓存好的 LocalVelocity（游戏线程已经拷好），不在这里读 Actor。
	constexpr float VerticalDeadZone = 0.15f;

	bIsAscending = LocalVelocity.Z > VerticalDeadZone;
	bIsDescending = LocalVelocity.Z < -VerticalDeadZone;
}
