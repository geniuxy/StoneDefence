// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SimpleAdvancedAnimationType.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleAdvancedAnimationBPLibrary.generated.h"

UCLASS()
class SIMPLEADVANCEDANIMATION_API USimpleAdvancedAnimationBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/**
	 * Create and register footik for specific objects.
	 * @param InCharacter				You need to enter a character based on acharacter.
	 * @param InBoneNames				If there are more than four skeletons, it may be a multi legged animal.
	 * @param TraceDistance				Additional distance of rays.
	 * @param InterpSpeed				How far does the ray go up at the bone point.
	 * @param InTraceStart				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * Return a footik ID.
	*/
	UFUNCTION(BlueprintCallable,Category = "SimpleAdvancedAnimation|FootIK")
	static int32 CreateFootIK(ACharacter* InCharacter, const TArray<FName>& InBoneNames, float TraceDistance = 50.f, float InterpSpeed = 18.f, float InTraceStart = 50.f);

	/**
	 * Find object offset.
	 * @param InKey					It's footik ID.
	 * @param InKeyName				Which bone point offset do you want to get.
	 * Return Offset value.
	*/
	UFUNCTION(BlueprintCallable, Category = "SimpleAdvancedAnimation|FootIK")
	static float FindOffset(const int32 InKey, const FName& InKeyName);

	/**
	 * Looking for more data.
	 * @param InKey					It's footik ID.
	 * @param InKeyName				Which bone point offset do you want to get.
	 * Returns the foot IK structure.
	*/
	UFUNCTION(BlueprintCallable, Category = "SimpleAdvancedAnimation|FootIK")
	static FFootIKInfo FindFootIKInfo(const int32 InKey, const FName& InKeyName);

	/**
	 * Buttock deviation.
	 * @param InOffsets				Calculate the final butt offset based on the cheap value @See FindOffset.
	 * Returns Bottom offset value.
	*/
	UFUNCTION(BlueprintCallable, Category = "SimpleAdvancedAnimation|FootIK")
	static float GetButtZOffset(const TArray<float> &InOffsets);

	/**
	 * Delete Advanced animation system. If you are developing in the editor, you'd better perform this step when closing the instance.
	*/
	UFUNCTION(BlueprintCallable, Category = "SimpleAdvancedAnimation|FootIK")
	static void Destroy();
};
