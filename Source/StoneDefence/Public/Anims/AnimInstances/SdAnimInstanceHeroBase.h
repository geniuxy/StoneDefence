// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdAnimInstanceCharacterBase.h"
#include "SdAnimInstanceHeroBase.generated.h"

class ASdCharacterHeroBase;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdAnimInstanceHeroBase : public USdAnimInstanceCharacterBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	ASdCharacterHeroBase* OwnerHero;

	/**********************************************************************/
	/*                          Figure Type Size                          */
	/**********************************************************************/
public:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetLegSize() const { return LegSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetWaistSize() const { return WaistSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetArmSize() const { return ArmSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetHeadSize() const { return HeadSize; }
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE int32 GetChestSize() const { return ChestSize; }

protected:
	int32 LegSize = 0;
	int32 WaistSize = 0;
	int32 ArmSize = 0;
	int32 HeadSize = 1;
	int32 ChestSize = 1;
};
