// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameInstanceSubsytems/SdGISubsystemCharacter.h"

#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "Frameworks/SdAssetManager.h"

USdGISubsystemCharacter* USdGISubsystemCharacter::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

		return UGameInstance::GetSubsystem<USdGISubsystemCharacter>(World->GetGameInstance());
	}

	return nullptr;
}

void USdGISubsystemCharacter::LoadCharacter(const FPrimaryAssetId& InAssetId, FOnCharacterDefinitionLoaded Callback)
{
	UAssetManager& AssetManager = UAssetManager::Get();

	AssetManager.LoadPrimaryAsset(
		InAssetId,
		TArray<FName>(),
		FStreamableDelegate::CreateLambda(
			[InAssetId, Callback]()
			{
				UPA_CharacterDefinition* Definition =
					Cast<UPA_CharacterDefinition>(UAssetManager::Get().GetPrimaryAssetObject(InAssetId));
				if (!Definition)
				{
					Callback.ExecuteIfBound(nullptr);
					return;
				}

				TArray<FSoftObjectPath> Paths;
				if (!Definition->GetCharacterIcon().IsNull())
				{
					Paths.Add(Definition->GetCharacterIcon().ToSoftObjectPath());
				}
				if (!Definition->GetCharacterClass().IsNull())
				{
					Paths.Add(Definition->GetCharacterClass().ToSoftObjectPath());
				}
				if (!Definition->GetDisplayAnimBP().IsNull())
				{
					Paths.Add(Definition->GetDisplayAnimBP().ToSoftObjectPath());
				}
				if (!Definition->GetGameAnimBP().IsNull())
				{
					Paths.Add(Definition->GetGameAnimBP().ToSoftObjectPath());
				}
				if (!Definition->GetAnimationSet().IsNull())
				{
					Paths.Add(Definition->GetAnimationSet().ToSoftObjectPath());
				}
				if (Paths.IsEmpty())
				{
					Callback.ExecuteIfBound(Definition);
					return;
				}

				TSharedPtr<FStreamableHandle> LoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
					Paths,
					FStreamableDelegate::CreateLambda(
						[Definition, Callback]()
						{
							Callback.ExecuteIfBound(Definition);
						}
					)
				);
			}
		)
	);
}
