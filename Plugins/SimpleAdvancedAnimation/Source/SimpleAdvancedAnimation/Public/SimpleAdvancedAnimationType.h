// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleAdvancedAnimationType.generated.h"

typedef int32 FSAAHandle;

USTRUCT(BlueprintType)
struct FFootIKInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SimpleAdvancedAnimation|FootIKInfo")
	float Offset;
};
