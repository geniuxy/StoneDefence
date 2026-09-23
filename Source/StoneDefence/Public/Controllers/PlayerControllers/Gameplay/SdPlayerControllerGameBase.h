// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/PlayerControllers/SdPlayerControllerBase.h"
#include "SdPlayerControllerGameBase.generated.h"

class UGeAbilitySystemComponent;
class ASdCharacterBase;
class USdWidgetGameHudMain;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API ASdPlayerControllerGameBase : public ASdPlayerControllerBase
{
	GENERATED_BODY()

public:
	ASdPlayerControllerGameBase();

protected:
	virtual void BeginPlay() override;

	// 只在Server上执行
	virtual void OnPossess(APawn* NewPawn) override;
	// 只在Client上执行,也会在Listen Server(P2P 没有玩家当主机)上执行(其中一个玩家同时充当"服务器"角色)
	virtual void AcknowledgePossession(APawn* NewPawn) override;
	
protected:
	UPROPERTY()
	TObjectPtr<ASdCharacterBase> OwningPlayerCharacter;

	UPROPERTY()
	TObjectPtr<UGeAbilitySystemComponent> OwningASC;

private:
	UPROPERTY()
	TSubclassOf<USdWidgetGameHudMain> GameHudMainClass;
	UPROPERTY()
	USdWidgetGameHudMain* GameHudMain = nullptr;
};
