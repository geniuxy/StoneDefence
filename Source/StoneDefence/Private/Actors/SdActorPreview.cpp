// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SdActorPreview.h"

#include "Camera/CameraComponent.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"


ASdActorPreview::ASdActorPreview()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComp"));

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComp");
	MeshComponent->SetupAttachment(GetRootComponent());

	ViewCameraComponent = CreateDefaultSubobject<UCameraComponent>("ViewCameraComp");
	ViewCameraComponent->SetupAttachment(GetRootComponent());
}

void ASdActorPreview::ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition)
{
	if (!CharacterDefinition) return;

	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	MeshComponent->SetSkeletalMesh(CharacterDefinition->LoadDisplayMesh());
	MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	MeshComponent->SetAnimClass(CharacterDefinition->LoadDisplayAnimationBP());

	ViewCameraComponent->SetRelativeLocation(CameraRelativeLocation);

	UpdateFigureTypeSize(CharacterDefinition->GetDefaultFigureSettings());
}

void ASdActorPreview::ClearCharacterDefinition()
{
	MeshComponent->SetSkeletalMesh(nullptr);
	MeshComponent->SetAnimClass(nullptr);
}

void ASdActorPreview::UpdateFigureTypeSize(ESdFigureType InType, int32 InValue)
{
	if (FigureSizeMap.Contains(InType))
	{
		FigureSizeMap[InType] = InValue;
	}
	else
	{
		FigureSizeMap.Add(InType, InValue);
	}
}

void ASdActorPreview::UpdateFigureTypeSize(TArray<FFaceSculptFigureTypeInfo> InFigureSettings)
{
	for (const FFaceSculptFigureTypeInfo& TypeInfo : InFigureSettings)
	{
		UpdateFigureTypeSize(TypeInfo.Type, TypeInfo.CurValue);
	}
}

void ASdActorPreview::UpdateFigureTypeSize(const FString& InFigureSizeStr)
{
	TArray<FString> StrFigureSizeArray;
	InFigureSizeStr.ParseIntoArray(StrFigureSizeArray, TEXT("|"));
	for (const FString& StrFigureSize : StrFigureSizeArray)
	{
		TArray<FString> StrFigureTypeAndSize;
		StrFigureSize.ParseIntoArray(StrFigureTypeAndSize, TEXT(","));
		if (StrFigureTypeAndSize.Num() != 2) continue;

		int TypeIndex = FCString::Atoi(*StrFigureTypeAndSize[0]);
		if (TypeIndex >= 0 && TypeIndex < static_cast<int>(ESdFigureType::FT_NUM))
		{
			UpdateFigureTypeSize(static_cast<ESdFigureType>(TypeIndex), FCString::Atoi(*StrFigureTypeAndSize[1]));
		}
	}
}

int32 ASdActorPreview::GetFigureSizeByType(ESdFigureType InType)
{
	// return FigureSizeMap.Contains(InType) ? FigureSizeMap[InType] : 0;
	return FigureSizeMap.FindRef(InType, 0);
}
