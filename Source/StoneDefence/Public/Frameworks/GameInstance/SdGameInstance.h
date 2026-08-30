// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SimpleNetManage.h"
#include "StoneDefenceNetCommonType.h"
#include "SdGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdGameInstance : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual void Shutdown() override;

public:
	void CreateClient();
	void LinkServer();

	FSimpleNetManage* GetClient() const;
	FSdUserData& GetUserData();
	FSdGateStatus GetGateStatus() const { return CachedGateStatus; }
	void SetGateStatus(const FSdGateStatus& InGateStatus) { CachedGateStatus = InGateStatus; }

private:
	FSimpleNetManage* Client;
	FSdUserData UserData;
	FSdGateStatus CachedGateStatus;
};
