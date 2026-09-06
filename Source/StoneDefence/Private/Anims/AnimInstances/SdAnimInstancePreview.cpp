// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstancePreview.h"

#include "Actors/SdActorPreview.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"

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

		LegSize = OwnerPreviewActor->GetFigureSizeByType(ESdFigureType::FT_LEG);
		WaistSize = OwnerPreviewActor->GetFigureSizeByType(ESdFigureType::FT_WAIST);
		ArmSize = OwnerPreviewActor->GetFigureSizeByType(ESdFigureType::FT_ARM);
	}
}
