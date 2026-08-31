// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Lobby/SdActorLobbyDisplay.h"

#include "Camera/CameraComponent.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"


ASdActorLobbyDisplay::ASdActorLobbyDisplay()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComp"));

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComp");
	MeshComponent->SetupAttachment(GetRootComponent());

	ViewCameraComponent = CreateDefaultSubobject<UCameraComponent>("ViewCameraComp");
	ViewCameraComponent->SetupAttachment(GetRootComponent());
}

void ASdActorLobbyDisplay::ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition)
{
	if (!CharacterDefinition) return;

	MeshComponent->SetSkeletalMesh(CharacterDefinition->LoadDisplayMesh());
	MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	MeshComponent->SetAnimClass(CharacterDefinition->LoadDisplayAnimationBP());
}

