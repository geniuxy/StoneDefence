// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeAnimInstanceLocoBase.h"
#include "GeAnimInstanceLocoFly.generated.h"

/**
 * 
 */
UCLASS()
class GENIUXYANIMATION_API UGeAnimInstanceLocoFly : public UGeAnimInstanceLocoBase
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

public:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE FVector GetFlySpeed() const { return FlySpeed; }

protected:
	FVector FlySpeed;
};
