// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/SdPawnLobby.h"


// Sets default values
ASdPawnLobby::ASdPawnLobby()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASdPawnLobby::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASdPawnLobby::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASdPawnLobby::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

