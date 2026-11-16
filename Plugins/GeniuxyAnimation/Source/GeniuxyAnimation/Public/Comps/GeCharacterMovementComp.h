// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GeCharacterMovementComp.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GENIUXYANIMATION_API UGeCharacterMovementComp : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UGeCharacterMovementComp();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
	) override;

protected:
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
};
