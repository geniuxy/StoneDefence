// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Hero/SdCharacterHeroBase.h"


// Sets default values
ASdCharacterHeroBase::ASdCharacterHeroBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASdCharacterHeroBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASdCharacterHeroBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASdCharacterHeroBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

