// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/SdPawnBase.h"


// Sets default values
ASdPawnBase::ASdPawnBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASdPawnBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASdPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASdPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

