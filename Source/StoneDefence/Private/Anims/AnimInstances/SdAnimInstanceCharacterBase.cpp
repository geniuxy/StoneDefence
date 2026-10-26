// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstanceCharacterBase.h"

#include "Characters/SdCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void USdAnimInstanceCharacterBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ASdCharacterBase>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}
}

void USdAnimInstanceCharacterBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 这里跑在游戏线程，可以安全地读角色上的数据；
	// 读出来缓存在本实例的成员里，工作线程只读缓存。
	//
	// bIsInAir / bFootIK 已经由 UGeAnimInstanceBase::CacheOwnerState 算好了，
	// 这里不需要再算一遍。
}

void USdAnimInstanceCharacterBase::CacheDerivedState()
{
	Super::CacheDerivedState();

	// 组件可能比 AnimInstance 晚就绪，补一次。
	if (!OwnerMovementComp && OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}
}

void USdAnimInstanceCharacterBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// 注意：这个函数跑在动画工作线程上，不要在这里读 OwnerCharacter / OwnerMovementComp。
	// 需要的值请在 NativeUpdateAnimation（游戏线程）里拷到成员变量。
}
