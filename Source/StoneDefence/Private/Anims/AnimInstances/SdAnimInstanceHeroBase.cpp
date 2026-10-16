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

void USdAnimInstanceHeroBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 这里跑在游戏线程，可以安全地读角色上的数据；
	// 读出来缓存在本实例的成员里，工作线程只读缓存。
	if (!OwnerHero) return;

	bNeedFaceSculpt = OwnerHero->IsNeedFaceSculpt();

	if (IFaceSculpting* SculptableHero = Cast<IFaceSculpting>(OwnerHero))
	{
		if (bNeedFaceSculpt)
		{
			LegSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_LEG);
			WaistSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_WAIST);
			ArmSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_ARM);
			HeadSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_HEAD);
			ChestSize = SculptableHero->GetFigureSizeByType(ESdFigureType::FT_CHEST);
		}
	}
}

void USdAnimInstanceHeroBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// 注意：这个函数跑在动画工作线程上。
	// 所有需要的数据都必须在 NativeUpdateAnimation（游戏线程）里拷好，
	// 这里绝对不要去读 OwnerHero / FigureSizeMap，那是数据竞争。
}
