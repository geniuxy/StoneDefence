// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdPlayerControllerBase.h"
#include "SdPlayerControllerLogin.generated.h"

class USdWidgetLoginMenu;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API ASdPlayerControllerLogin : public ASdPlayerControllerBase
{
	GENERATED_BODY()

public:
	ASdPlayerControllerLogin();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<USdWidgetLoginMenu> LoginMenuClass;
	UPROPERTY()
	USdWidgetLoginMenu* LoginMenu = nullptr;
};
