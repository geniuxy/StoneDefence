// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SdGISubsystemEnum.generated.h"

/**
 * 用于注册所有Enum的DisplayName
 * 解决USdFunctionLibraryCommon::GetDisplayValueOfEnum在打包后无法正常使用的问题
 */
UCLASS()
class STONEDEFENCE_API USdGISubsystemEnum : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
