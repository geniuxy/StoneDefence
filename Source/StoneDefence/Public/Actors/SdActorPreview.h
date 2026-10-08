// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/SdActorBase.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "Interfaces/FaceSculpting.h"
#include "StoneDefence/StoneDefence.h"
#include "SdActorPreview.generated.h"

class UCameraComponent;

UCLASS()
class STONEDEFENCE_API ASdActorPreview : public ASdActorBase, public IFaceSculpting
{
	GENERATED_BODY()

public:
	ASdActorPreview();

	void LoadCharacterDefinition(const FPrimaryAssetId& InAssetId);
	void ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition);
	void ClearCharacterDefinition();

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Character Display")
	USkeletalMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Character Display")
	UCameraComponent* ViewCameraComponent;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FVector CameraRelativeLocation = FVector(200.f, 72.f, 73.f);

	bool IsModifying = false;

public:
	DATA_ACCESSOR(bool, IsModifying)
};
