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

		if (IFaceSculpting* SculptableActor = Cast<IFaceSculpting>(OwnerPreviewActor))
		{
			LegSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_LEG);
			WaistSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_WAIST);
			ArmSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_ARM);
			HeadSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_HEAD);
			ChestSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_CHEST);
		}
	}
}
