// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdAnimInstanceBase.h"
#include "SdAnimInstanceCharacterBase.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdAnimInstanceCharacterBase : public USdAnimInstanceBase
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsInAir() const { return bIsInAir; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

protected:
	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;
	
	bool bIsInAir = false;
	float Speed;
};
