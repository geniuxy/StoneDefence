// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GeniuxyAnimationType.h"
#include "Animation/AnimInstance.h"
#include "GeAnimInstanceBase.generated.h"

enum class ECharacterActionState : uint8;
class UCharacterMovementComponent;
class USkeletalMeshComponent;
/**
 * 
 */
UCLASS()
class GENIUXYANIMATION_API UGeAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	USkeletalMeshComponent* OwnerSkeletalMeshComp;

	/**********************************************************************/
	/*                             Locomotion                             */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE ECharacterActionState GetActionState() const { return ActionState; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsInCombat() const { return ActionState == ECharacterActionState::COMBAT; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsFlying() const { return ActionState == ECharacterActionState::FLIGHT; }

protected:
	UPROPERTY()
	APawn* OwnerPawn;
	
	float Speed;
	ECharacterActionState ActionState;

protected:
	void OwnerCombatTagUpdated(const FGameplayTag Tag, int32 NewCount);
	void OwnerFlyTagUpdated(const FGameplayTag Tag, int32 NewCount);

	/**********************************************************************/
	/*                              Foot IK                               */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLeftOffset() const { return LeftOffset; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetRightOffset() const { return RightOffset; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetButtZOffset() const { return ButtZOffset; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsFootIK() const { return bFootIK; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool HasIKBone() const { return bHasIKBone; }

protected:
	virtual void InitFootIKId(ACharacter* OwnerCharacter);
	void CalcFootIKOffset();

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	bool bCanFootIK;

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	bool bHasIKBone;

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
