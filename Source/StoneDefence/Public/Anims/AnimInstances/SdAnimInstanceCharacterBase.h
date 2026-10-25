// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/Instances/GeAnimInstanceBase.h"
#include "SdAnimInstanceCharacterBase.generated.h"

class ASdCharacterBase;
class UCharacterMovementComponent;
/**
 * 
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

public:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsInAir() const { return bIsInAir; }

protected:
	bool bIsInAir = false;
};
