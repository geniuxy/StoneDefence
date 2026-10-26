// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/Data/SdAnimationSetup.h"

#include "Animation/AnimInstance.h"
#include "Data/GeLocomotionAnimSet.h"

TArray<TSubclassOf<UAnimInstance>> USdAnimationSetup::LoadLocomotionLayerClasses() const
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

UGeLocomotionAnimSet* USdAnimationSetup::LoadLocomotionAnimSet() const
{
	return LocomotionAnimSet.LoadSynchronous();
}
