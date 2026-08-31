// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/SdAssetManager.h"

#include "SdDebugHelper.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"

USdAssetManager& USdAssetManager::Get()
{
	USdAssetManager* Singleton = Cast<USdAssetManager>(GEngine->AssetManager.Get());
	if (Singleton)
	{
		return *Singleton;
	}

	Debug::Print(TEXT("AssetManager的类型需要是USdAssetManager"));
	return *NewObject<USdAssetManager>();
}

void USdAssetManager::LoadCharacterDefinitions(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(
		UPA_CharacterDefinition::GetCharacterDefinitionAssetType(),
		TArray<FName>(),
		LoadFinishedCallback
	);
}

bool USdAssetManager::GetLoadedCharacterDefinitions(TArray<UPA_CharacterDefinition*>& OutDefinitions)
{
	TArray<UObject*> LoadedObjects;
	bool bLoaded = GetPrimaryAssetObjectList(UPA_CharacterDefinition::GetCharacterDefinitionAssetType(), LoadedObjects);
	if (bLoaded)
	{
		for (UObject* LoadedObject : LoadedObjects)
		{
			OutDefinitions.Add(Cast<UPA_CharacterDefinition>(LoadedObject));
		}
	}
	return bLoaded;
}

void USdAssetManager::LoadCharacterDefinition(const FName& CharacterAssetName, const FStreamableDelegate& OnLoaded)
{
	const FPrimaryAssetId AssetId(
		UPA_CharacterDefinition::GetCharacterDefinitionAssetType(), 
		CharacterAssetName
	);
    
	// LoadPrimaryAsset 会自动处理：如果已加载则立即回调，未加载则异步加载
	LoadPrimaryAsset(AssetId, TArray<FName>(), OnLoaded);
}

UPA_CharacterDefinition* USdAssetManager::GetCharacterDefinition(const FName& CharacterAssetName) const
{
	const FPrimaryAssetId AssetId(
		UPA_CharacterDefinition::GetCharacterDefinitionAssetType(), 
		CharacterAssetName
	);
	return Cast<UPA_CharacterDefinition>(GetPrimaryAssetObject(AssetId));
}

UPA_CharacterDefinition* USdAssetManager::GetCharacterDefinitionById(const FPrimaryAssetId& AssetId) const
{
	return Cast<UPA_CharacterDefinition>(GetPrimaryAssetObject(AssetId));
}

