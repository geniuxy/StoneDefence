// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/SdActorBase.h"
#include "StoneDefence/StoneDefence.h"
#include "SdActorPreview.generated.h"

struct FFaceSculptFigureTypeInfo;
enum class ESdFigureType : uint8;
class UCameraComponent;
class UPA_CharacterDefinition;

UCLASS()
class STONEDEFENCE_API ASdActorPreview : public ASdActorBase
{
	GENERATED_BODY()

public:
	ASdActorPreview();

	void ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition);
	void ClearCharacterDefinition();

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Character Display")
	USkeletalMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Character Display")
	UCameraComponent* ViewCameraComponent;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FVector CameraRelativeLocation = FVector(200.f, 72.f, 73.f);

	bool IsModifying = false;

public:
	DATA_ACCESSOR(bool, IsModifying)

	/**********************************************************************/
	/*                          Figure Type Size                          */
	/**********************************************************************/
public:
	void UpdateFigureTypeSize(ESdFigureType InType, int32 InValue);
	void UpdateFigureTypeSize(TArray<FFaceSculptFigureTypeInfo> InFigureSettings);
	void UpdateFigureTypeSize(const FString& InFigureSizeStr);
	int32 GetFigureSizeByType(ESdFigureType InType);
	
protected:
	TMap<ESdFigureType, int32> FigureSizeMap;
};
