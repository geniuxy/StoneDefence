// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SdAssetManager.generated.h"

class UPA_CharacterDefinition;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static USdAssetManager& Get();

	void LoadCharacterDefinitions(const FStreamableDelegate& LoadFinishedCallback);
	bool GetLoadedCharacterDefinitions(TArray<UPA_CharacterDefinition*>& OutDefinitions);

	/* ========== 精准获取单个角色（核心） ========== */
    
	/** 异步加载指定角色定义 */
	void LoadCharacterDefinition(const FName& CharacterAssetName, const FStreamableDelegate& OnLoaded);
    
	/** 同步获取（仅当已加载时有效，否则返回 nullptr） */
	UPA_CharacterDefinition* GetCharacterDefinition(const FName& CharacterAssetName) const;
    
	/** 通过 FPrimaryAssetId 直接获取（更通用） */
	UPA_CharacterDefinition* GetCharacterDefinitionById(const FPrimaryAssetId& AssetId) const;
};
