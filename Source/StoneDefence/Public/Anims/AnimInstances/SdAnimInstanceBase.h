// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SdAnimInstanceBase.generated.h"

class USkeletalMeshComponent;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

protected:
	UPROPERTY()
	USkeletalMeshComponent* OwnerSkeletalMeshComp;
};
