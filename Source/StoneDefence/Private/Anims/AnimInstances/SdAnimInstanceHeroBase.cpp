// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstanceHeroBase.h"

#include "Characters/Hero/SdCharacterHeroBase.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"

void USdAnimInstanceHeroBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwnerCharacter)
	{
		OwnerHero = Cast<ASdCharacterHeroBase>(OwnerCharacter);
	}
}

void USdAnimInstanceHeroBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (OwnerHero)
	{
		LegSize = OwnerHero->GetFigureSizeByType(ESdFigureType::FT_LEG);
		WaistSize = OwnerHero->GetFigureSizeByType(ESdFigureType::FT_WAIST);
		ArmSize = OwnerHero->GetFigureSizeByType(ESdFigureType::FT_ARM);
		HeadSize = OwnerHero->GetFigureSizeByType(ESdFigureType::FT_HEAD);
		ChestSize = OwnerHero->GetFigureSizeByType(ESdFigureType::FT_CHEST);
	}
}
