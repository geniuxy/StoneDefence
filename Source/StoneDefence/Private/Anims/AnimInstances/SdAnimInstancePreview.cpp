// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstancePreview.h"

#include "Actors/SdActorPreview.h"
#include "Interfaces/FaceSculpting.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"

void USdAnimInstancePreview::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwnerSkeletalMeshComp)
	{
		OwnerPreviewActor = Cast<ASdActorPreview>(OwnerSkeletalMeshComp->GetOwner());
	}

	Montage_Play(EnterAnim);
}

void USdAnimInstancePreview::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 游戏线程：读预览 Actor 的数据并缓存，工作线程只读缓存。
	if (!OwnerPreviewActor) return;

	bIsModifying = OwnerPreviewActor->GetIsModifying();

	if (IFaceSculpting* SculptableActor = Cast<IFaceSculpting>(OwnerPreviewActor))
	{
		LegSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_LEG);
		WaistSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_WAIST);
		ArmSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_ARM);
		HeadSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_HEAD);
		ChestSize = SculptableActor->GetFigureSizeByType(ESdFigureType::FT_CHEST);
	}
}

void USdAnimInstancePreview::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// 注意：这个函数跑在动画工作线程上，不要在这里读 OwnerPreviewActor。
	// 需要的值请在 NativeUpdateAnimation（游戏线程）里拷到成员变量。
}
