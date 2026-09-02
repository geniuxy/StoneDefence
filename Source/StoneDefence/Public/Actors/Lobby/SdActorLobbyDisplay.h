// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/SdActorBase.h"
#include "SdActorLobbyDisplay.generated.h"

class UCameraComponent;
class UPA_CharacterDefinition;

UCLASS()
class STONEDEFENCE_API ASdActorLobbyDisplay : public ASdActorBase
{
	GENERATED_BODY()

public:
	ASdActorLobbyDisplay();

	void ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition);
	void ClearCharacterDefinition();

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Character Display")
	USkeletalMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Character Display")
	UCameraComponent* ViewCameraComponent;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FVector CameraRelativeLocation = FVector(200.f, 72.f, 73.f);
};
