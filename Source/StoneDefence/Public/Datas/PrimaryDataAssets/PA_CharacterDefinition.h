// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_CharacterDefinition.generated.h"

class ASdCharacterBase;

UENUM()
enum class ESdFigureType : uint8
{
	FT_LEG UMETA(DisplayName = "腿"),
	FT_WAIST UMETA(DisplayName = "腰"),
	FT_ARM UMETA(DisplayName = "手臂"),
	FT_NUM
};
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API UPA_CharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	static FPrimaryAssetType GetCharacterDefinitionAssetType();

	UTexture2D* LoadIcon() const;
	TSubclassOf<ASdCharacterBase> LoadCharacterClass() const;
	TSubclassOf<UAnimInstance> LoadDisplayAnimationBP() const;
	USkeletalMesh* LoadDisplayMesh() const;

private:
	UPROPERTY(EditDefaultsOnly, Category="Character")
	FString CharacterName;

	UPROPERTY(EditDefaultsOnly, Category="Character")
	TSoftObjectPtr<UTexture2D> CharacterIcon;

	UPROPERTY(EditDefaultsOnly, Category="Character")
	TSoftClassPtr<ASdCharacterBase> CharacterClass;

	UPROPERTY(EditDefaultsOnly, Category="Character")
	TSoftClassPtr<UAnimInstance> DisplayAnimBP;

	UPROPERTY(EditDefaultsOnly, Category="Character")
	TMap<ESdFigureType, float> DefaultFigureSettings; 

public:
	FORCEINLINE FString GetCharacterDisplayName() const { return CharacterName; }
	FORCEINLINE TMap<ESdFigureType, float> GetDefaultFigureSettings() const { return DefaultFigureSettings; }
};
