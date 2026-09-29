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
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLeftOffset() const { return LeftOffset; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetRightOffset() const { return RightOffset; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetButtZOffset() const { return ButtZOffset; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsFootIK() const { return bFootIK; }

protected:
	UPROPERTY()
	USkeletalMeshComponent* OwnerSkeletalMeshComp;

	/**********************************************************************/
	/*                              Foot IK                               */
	/**********************************************************************/
protected:
	virtual void InitFootIKId(ACharacter* OwnerCharacter);
	void CalcFootIKOffset();

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	bool bCanFootIK;

	bool bFootIK;

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	FName LeftBoneName = TEXT("foot_l");

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	FName RightBoneName = TEXT("foot_r");

	float LeftOffset;
	float RightOffset;
	float ButtZOffset;
	int32 FootIKId = INDEX_NONE;
	TArray<FName> BoneNames;

public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetFootIKOffset(const FName &InBoneName);
};
