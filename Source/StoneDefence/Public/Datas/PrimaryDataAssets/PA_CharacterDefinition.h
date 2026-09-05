// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StoneDefence/StoneDefence.h"
#include "PA_CharacterDefinition.generated.h"

class ASdCharacterBase;

UENUM()
enum class ESdFigureType : uint8
{
	FT_LEG UMETA(DisplayName = "腿长"),
	FT_WAIST UMETA(DisplayName = "腰长"),
	FT_ARM UMETA(DisplayName = "臂长"),
	FT_NUM
};

USTRUCT(BlueprintType)
struct FFaceSculptFigureTypeInfo // 捏脸身材塑造时相关的信息
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ESdFigureType Type;

	UPROPERTY(EditAnywhere)
	float DefaultValue; // 0~1之间

	UPROPERTY(EditAnywhere)
	int MaxValue;

	UPROPERTY()
	int CurValue;
	
	DATA_ACCESSOR(int, CurValue)
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
	TArray<FFaceSculptFigureTypeInfo> DefaultFigureSettings; 

public:
	FORCEINLINE FString GetCharacterDisplayName() const { return CharacterName; }
	FORCEINLINE TArray<FFaceSculptFigureTypeInfo> GetDefaultFigureSettings() const { return DefaultFigureSettings; }
};
