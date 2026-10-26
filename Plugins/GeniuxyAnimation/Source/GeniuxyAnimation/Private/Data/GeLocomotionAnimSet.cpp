// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/GeLocomotionAnimSet.h"

#include "Animation/AnimSequenceBase.h"
#include "Animation/BlendSpace.h"

void UGeLocomotionAnimSet::PreloadAll()
{
	// 只在游戏线程执行。LoadSynchronous 内部有缓存，重复调用不会重复 IO。
	// 之后所有 Get* 都读上面这些缓存指针，动画工作线程不会再触发任何加载。
	CachedFlyBlendSpace = FlyBlendSpace.LoadSynchronous();
	CachedFlyStartSequence = FlyStartSequence.LoadSynchronous();
	CachedFlyStopSequence = FlyStopSequence.LoadSynchronous();

	CachedSwimBlendSpace = SwimBlendSpace.LoadSynchronous();
	CachedSwimStartSequence = SwimStartSequence.LoadSynchronous();

	CachedClimbBlendSpace = ClimbBlendSpace.LoadSynchronous();
	CachedClimbUpSequence = ClimbUpSequence.LoadSynchronous();
	CachedClimbDownSequence = ClimbDownSequence.LoadSynchronous();
}
