// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SdCharacterBase.h"
#include "SdCharacterHeroBase.generated.h"

UCLASS()
class STONEDEFENCE_API ASdCharacterHeroBase : public ASdCharacterBase
{
	GENERATED_BODY()

public:
	ASdCharacterHeroBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
