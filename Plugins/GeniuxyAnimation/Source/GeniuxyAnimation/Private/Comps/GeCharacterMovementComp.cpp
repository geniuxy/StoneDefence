// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/GeCharacterMovementComp.h"

UGeCharacterMovementComp::UGeCharacterMovementComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGeCharacterMovementComp::BeginPlay()
{
	Super::BeginPlay();
}


void UGeCharacterMovementComp::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGeCharacterMovementComp::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	
}

