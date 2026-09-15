// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SdCharacterBase.h"


ASdCharacterBase::ASdCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASdCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASdCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASdCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

