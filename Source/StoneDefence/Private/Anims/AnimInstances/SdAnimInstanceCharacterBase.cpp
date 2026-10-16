// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstanceCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/SdCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/StatsTags.h"

void USdAnimInstanceCharacterBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ASdCharacterBase>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(StatsTags::Ge_Stats_InCombat).AddUObject(
			this, &ThisClass::OwnerCombatTagUpdated
		);
	}
}

void USdAnimInstanceCharacterBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 游戏线程：Actor / MovementComponent 的数据只能在这里读，
	// 读出来缓存在成员里给动画工作线程用。
	if (OwnerCharacter)
	{
		const FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();
	}

	if (OwnerMovementComp)
	{
		bIsInAir = OwnerMovementComp->IsFalling();
	}

	bFootIK = bCanFootIK && !bIsInAir;
}

void USdAnimInstanceCharacterBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// 注意：这个函数跑在动画工作线程上，不要在这里读 OwnerCharacter / OwnerMovementComp。
	// 需要的值请在 NativeUpdateAnimation（游戏线程）里拷到成员变量。
}

void USdAnimInstanceCharacterBase::OwnerCombatTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	bIsInCombat = NewCount != 0;
}
