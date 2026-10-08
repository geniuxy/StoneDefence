// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstanceHeroBase.h"

#include "Characters/Hero/SdCharacterHeroBase.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "Interfaces/FaceSculpting.h"

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
		if (IFaceSculpting* SculptableHero = Cast<IFaceSculpting>(OwnerHero))
		{
			LegSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_LEG);
			WaistSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_WAIST);
			ArmSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_ARM);
			HeadSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_HEAD);
			ChestSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_CHEST);
		}
	}
}
