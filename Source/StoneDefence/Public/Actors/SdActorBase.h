// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SdActorBase.generated.h"

UCLASS()
class STONEDEFENCE_API ASdActorBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASdActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
