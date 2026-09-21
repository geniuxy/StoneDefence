// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameInstanceSubsytems/SdGISubsystemEnum.h"

#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "SdTypes/SdEnumDisplayHelper.h"
#include "Widgets/Lobby/FaceSculpt/SdButtonFaceSculptType.h"

void USdGISubsystemEnum::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// NSLOCTEXT的最后一个是兜底文本，如果没有多国化翻译的话，就用最后一个

	TEnumDisplayRegistrar<ESdFaceSculptPageType>::Add(
		ESdFaceSculptPageType::FSP_FIGURE, NSLOCTEXT("FaceSculpt", "Figure", "身材")
	);
	TEnumDisplayRegistrar<ESdFaceSculptPageType>::Add(
		ESdFaceSculptPageType::FSP_FACE, NSLOCTEXT("FaceSculpt", "Face", "容貌")
	);
	TEnumDisplayRegistrar<ESdFaceSculptPageType>::Add(
		ESdFaceSculptPageType::FSP_TALENT, NSLOCTEXT("FaceSculpt", "Talent", "天赋")
	);
	TEnumDisplayRegistrar<ESdFaceSculptPageType>::Register();

	TEnumDisplayRegistrar<ESdFigureType>::Add(ESdFigureType::FT_LEG, NSLOCTEXT("FigureType", "Leg", "腿长"));
	TEnumDisplayRegistrar<ESdFigureType>::Add(ESdFigureType::FT_WAIST, NSLOCTEXT("FigureType", "Waist", "腰长"));
	TEnumDisplayRegistrar<ESdFigureType>::Add(ESdFigureType::FT_ARM, NSLOCTEXT("FigureType", "Arm", "臂长"));
	TEnumDisplayRegistrar<ESdFigureType>::Add(ESdFigureType::FT_HEAD, NSLOCTEXT("FigureType", "Head", "头大"));
	TEnumDisplayRegistrar<ESdFigureType>::Add(ESdFigureType::FT_CHEST, NSLOCTEXT("FigureType", "Chest", "胸大"));
	TEnumDisplayRegistrar<ESdFigureType>::Register();
}
