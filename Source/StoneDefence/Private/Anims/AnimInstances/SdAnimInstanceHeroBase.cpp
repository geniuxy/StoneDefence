// Fill out your copyright notice in the Description page of Project Settings.


#include "Anims/AnimInstances/SdAnimInstanceHeroBase.h"

#include "Comps/Locomotions/GeLocomotionComponent.h"
#include "Data/GeAnimationSetup.h"
#include "Characters/Hero/SdCharacterHeroBase.h"
#include "Interfaces/FaceSculpting.h"

namespace
{
	/**
	 * 比较两组层实现类是否一致。
	 * 注意：TSubclassOf 没有定义 operator==，所以不能用 TArray 的 operator==，
	 * 必须逐个比 Get()。
	 */
	bool AreSameLayerClasses(
		const TArray<TSubclassOf<UAnimInstance>>& InA,
		const TArray<TSubclassOf<UAnimInstance>>& InB)
	{
		if (InA.Num() != InB.Num())
		{
			return false;
		}

		for (int32 Index = 0; Index < InA.Num(); ++Index)
		{
			if (InA[Index].Get() != InB[Index].Get())
			{
				return false;
			}
		}

		return true;
	}
}

void USdAnimInstanceHeroBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (!OwnerHero)
	{
		OwnerHero = Cast<ASdCharacterHeroBase>(OwnerCharacter);
	}
	if (!OwnerHero)
	{
		OwnerHero = Cast<ASdCharacterHeroBase>(OwnerPawn);
	}

	// 【重要】不要在这里调 RefreshLocomotionLayers()。
	//
	// UAnimInstance::InitializeAnimation 的顺序是：
	//     NativeInitializeAnimation()  <- 我们现在在这里，动画图节点还没初始化
	//     BlueprintInitializeAnimation()
	//     InitializeRootNode()
	//     InitializeGroupedLayers()    <- Layer 节点到这里才就绪
	//
	// 引擎在 UAnimInstance::PerformLinkedLayerOverlayOperation 里对这种情况有明确告警：
	//   "Performing linked layer operations in initialization may produce unexpected results,
	//    please use the BlueprintLinkedAnimationLayersInitialized event to perform linked layer operations."
	//
	// 所以绑层交给 ASdCharacterBase 在两个安全时机驱动：
	//   1. BeginPlay 时（AnimInstance 已存在则直接绑）
	//   2. USkeletalMeshComponent::OnAnimInitialized 回调（AnimInstance 后创建的情况）
}

void USdAnimInstanceHeroBase::RefreshLocomotionLayers()
{
	// 先解绑：把所有 Layer 节点还原成主 ABP 里配置的 Instance Class（兜底）。
	// 传一个空的 TSubclassOf 是引擎文档明确支持的重置方式（对应蓝图里的 null class）。
	LinkAnimClassLayers(TSubclassOf<UAnimInstance>());

	TArray<TSubclassOf<UAnimInstance>> DesiredLayerClasses;

	// 优先用角色自己的配置。配置挂在 UGeLocomotionComponent 上（不是角色上），
	// 这样主实例和 layer 实例读的是同一份数据，也不用多绕一层角色。
	if (const UGeLocomotionComponent* Comp = UGeLocomotionComponent::Find(OwnerPawn))
	{
		if (const UGeAnimationSetup* Setup = Comp->GetAnimationSetup())
		{
			DesiredLayerClasses = Setup->LoadLocomotionLayerClasses();
		}
	}

	// 没有就回退到类默认值（在 ABP_SdHeroBase 的 Class Defaults 里填一次）
	if (DesiredLayerClasses.Num() == 0)
	{
		DesiredLayerClasses.Reserve(DefaultLocomotionLayerClasses.Num());
		for (const TSoftClassPtr<UAnimInstance>& SoftClass : DefaultLocomotionLayerClasses)
		{
			if (UClass* LoadedClass = SoftClass.LoadSynchronous())
			{
				DesiredLayerClasses.Add(LoadedClass);
			}
		}
	}

	// 目标类和当前一致就不重建 linked instance，避免无谓的开销和姿态闪断。
	// 只比较「已经按这个顺序绑过」，所以重复调用 Refresh 是安全的。
	if (AreSameLayerClasses(DesiredLayerClasses, AppliedLayerClasses))
	{
		return;
	}

	AppliedLayerClasses = MoveTemp(DesiredLayerClasses);

	// 一个类可以实现多个层：LinkAnimClassLayers 会扫描所有 Layer 节点，
	// 把该类的每个层绑定到对应的节点上。
	for (const TSubclassOf<UAnimInstance>& LayerClass : AppliedLayerClasses)
	{
		if (LayerClass)
		{
			LinkAnimClassLayers(LayerClass);
		}
	}
}

void USdAnimInstanceHeroBase::CacheDerivedState()
{
	Super::CacheDerivedState();

	// 这里跑在游戏线程（由 UGeAnimInstanceBase::NativeUpdateAnimation 调用），
	// 可以安全地读角色数据，读出来的值缓存在本实例，工作线程只读缓存。
	if (!OwnerHero)
	{
		return;
	}

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
	//
	// 同理：LinkAnimClassLayers() 也绝对不能在这里调。
}
