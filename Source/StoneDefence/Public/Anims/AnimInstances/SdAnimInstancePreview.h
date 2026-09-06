// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdAnimInstanceBase.h"
#include "SdAnimInstancePreview.generated.h"

class ASdActorPreview;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdAnimInstancePreview : public USdAnimInstanceBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsModifying() const { return bIsModifying; }

protected:
	UPROPERTY(EditDefaultsOnly, Category="Start Anim")
	UAnimMontage* EnterAnim;

	UPROPERTY()
	ASdActorPreview* OwnerPreviewActor;

	bool bIsModifying = false;

	/**********************************************************************/
	/*                          Figure Type Size                          */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLegSize() const { return LegSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetWaistSize() const { return WaistSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetArmSize() const { return ArmSize; }

protected:
	float LegSize = 0.f;
	float WaistSize = 0.f;
	float ArmSize = 0.f;
};
