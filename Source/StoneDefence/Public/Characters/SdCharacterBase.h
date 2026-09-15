// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SdCharacterBase.generated.h"

UCLASS()
class STONEDEFENCE_API ASdCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASdCharacterBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
