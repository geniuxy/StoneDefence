// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstancePreview.h"

#include "Actors/SdActorPreview.h"

void USdAnimInstancePreview::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwnerSkeletalMeshComp)
	{
		OwnerPreviewActor = Cast<ASdActorPreview>(OwnerSkeletalMeshComp->GetOwner());
	}

	Montage_Play(EnterAnim);
}

void USdAnimInstancePreview::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (OwnerPreviewActor)
	{
		bIsModifying = OwnerPreviewActor->GetIsModifying();
	}
}
