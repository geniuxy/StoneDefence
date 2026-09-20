// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GeAnimationComponent.generated.h"


UCLASS(ClassGroup=(Animation), meta=(BlueprintSpawnableComponent))
class GENIUXYANIMATION_API UGeAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGeAnimationComponent();
};
