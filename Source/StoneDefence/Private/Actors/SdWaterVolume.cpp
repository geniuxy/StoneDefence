// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SdWaterVolume.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Effects/GrantTag/GE_Stats_Swim.h"
#include "GeniuxyDebugHelper.h"

ASdWaterVolume::ASdWaterVolume()
{
	// 关键开关：告诉引擎「这是水」。
	// UCharacterMovementComponent::PhysicsVolumeChanged 里只有 bWaterVolume 为真才会
	// 自动 SetMovementMode(MOVE_Swimming)；PhysSwimming 每帧也靠它确认自己还在水里。
	bWaterVolume = true;

	// 比默认物理体积（Priority 0）高，避免被别的体积抢占判定。
	Priority = 10;

	// 默认用原生 GE：无需在 Content 里先建资产，拖进关卡就能用。
	SwimGameplayEffect = UGE_Stats_Swim::StaticClass();
}

void ASdWaterVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// GE 是跟着 ASC 走的，体积没了不需要主动摘；这里只把句柄表清掉。
	SwimEffectHandles.Reset();

	Super::EndPlay(EndPlayReason);
}

bool ASdWaterVolume::IsSwimTagApplied(AActor* Other) const
{
	return Other && SwimEffectHandles.Contains(Other);
}

void ASdWaterVolume::ActorEnteredVolume(AActor* Other)
{
	Super::ActorEnteredVolume(Other);

	ApplySwimTag(Other);
}

void ASdWaterVolume::ActorLeavingVolume(AActor* Other)
{
	Super::ActorLeavingVolume(Other);

	// 被优先级更高的体积接管时也会收到一次「离开」，但人其实还在水里。
	// 这时候千万别摘 tag，否则就成了「泡在水里却在播走路动画」。
	if (Other && EncompassesPoint(Other->GetActorLocation()))
	{
		return;
	}

	RemoveSwimTag(Other);
}

void ASdWaterVolume::ApplySwimTag(AActor* Other)
{
	if (!Other)
	{
		return;
	}

	// 只在服务器改状态。
	// GE 授予的 tag 会随 ActiveGameplayEffects 复制到各端，客户端收到 tag 事件后
	// UGeLocomotionComponent 自然会切 Swim；反过来在客户端本地加 GE 服务器不认，
	// 还会被复制数据覆盖，属于自找麻烦。
	if (!HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other);
	if (!ASC)
	{
		// 没有 GAS 的 actor（箱子 / 纯物理单位）：物理层照样会被引擎切成游泳，
		// 只是没有「模式」这层状态，不需要也不应该报错。
		return;
	}

	// 清掉已经销毁的 actor 留下的句柄：角色在水里被销毁时收不到「离开体积」通知。
	for (auto It = SwimEffectHandles.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid())
		{
			It.RemoveCurrent();
		}
	}

	if (SwimEffectHandles.Contains(Other))
	{
		// 已经挂着（例如同一帧被多次通知），不重复上。
		return;
	}

	if (!SwimGameplayEffect)
	{
		Debug::Print(
			FString::Printf(TEXT("[SdWaterVolume] %s 没配 SwimGameplayEffect，入水后不会切游泳模式"), *GetName()),
			TEXT("Water.NoGE"), 8.f, FColor::Red);
		return;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);

	const FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectToSelf(
		SwimGameplayEffect->GetDefaultObject<UGameplayEffect>(), 1.f, Context);

	if (!Handle.WasSuccessfullyApplied())
	{
		Debug::Print(
			FString::Printf(TEXT("[SdWaterVolume] %s 给 %s 挂 Ge.Stats.Swim 失败"), *GetName(), *Other->GetName()),
			TEXT("Water.ApplyFailed"), 8.f, FColor::Red);
		return;
	}

	SwimEffectHandles.Add(Other, Handle);

	if (bPrintDebug)
	{
		Debug::Print(
			FString::Printf(TEXT("[SdWaterVolume] %s 入水 → Ge.Stats.Swim"), *Other->GetName()),
			TEXT("Water.Enter"), 4.f, FColor::Cyan);
	}
}

void ASdWaterVolume::RemoveSwimTag(AActor* Other)
{
	if (!Other || !HasAuthority())
	{
		return;
	}

	FActiveGameplayEffectHandle Handle;
	if (!SwimEffectHandles.RemoveAndCopyValue(Other, Handle))
	{
		// 本体积没给他挂过（例如进来的不是 GAS 角色），无事可做。
		return;
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	if (bPrintDebug)
	{
		Debug::Print(
			FString::Printf(TEXT("[SdWaterVolume] %s 出水 → 摘掉 Ge.Stats.Swim"), *Other->GetName()),
			TEXT("Water.Exit"), 4.f, FColor::Cyan);
	}
}
