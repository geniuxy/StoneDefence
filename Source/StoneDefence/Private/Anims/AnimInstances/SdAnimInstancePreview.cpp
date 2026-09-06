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

	if (!USdGISubsystemLobby::Get(this)) return;
	USdGISubsystemLobby* LobbySubsystem = USdGISubsystemLobby::Get(this);
	
	if (OwnerPreviewActor)
	{
		bIsModifying = OwnerPreviewActor->GetIsModifying();

		if (LobbySubsystem)
		{
			LegSize = LobbySubsystem->GetCachedFigureValueByType(ESdFigureType::FT_LEG);
			WaistSize = LobbySubsystem->GetCachedFigureValueByType(ESdFigureType::FT_WAIST);
			ArmSize = LobbySubsystem->GetCachedFigureValueByType(ESdFigureType::FT_ARM);
		}
	}
}
