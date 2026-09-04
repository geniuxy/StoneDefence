// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstanceBase.h"

#include "Components/SkeletalMeshComponent.h"

void USdAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerSkeletalMeshComp = GetSkelMeshComponent();
}
