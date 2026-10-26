// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/GeAnimationSetup.h"

#include "Animation/AnimInstance.h"

TArray<TSubclassOf<UAnimInstance>> UGeAnimationSetup::LoadLocomotionLayerClasses() const
{
	TArray<TSubclassOf<UAnimInstance>> Result;
	Result.Reserve(LocomotionLayerClasses.Num());

	for (const TSoftClassPtr<UAnimInstance>& SoftClass : LocomotionLayerClasses)
	{
		if (UClass* LoadedClass = SoftClass.LoadSynchronous())
		{
			Result.Add(LoadedClass);
		}
	}

	return Result;
}
