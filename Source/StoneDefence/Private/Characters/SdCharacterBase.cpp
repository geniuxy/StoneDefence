// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SdCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "Comps/GeAnimationComponent.h"
#include "Anims/AnimInstances/SdAnimInstanceHeroBase.h"
#include "Types/GeAnimationPayloads.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GeniuxyDebugHelper.h"
#include "AttributeSets/GeAttributeSetBase.h"
#include "Comps/Locomotions/GeLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/CharacterAnimationSet.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemCharacter.h"
#include "Tags/EventTags.h"
#include "Tags/CharacterTags.h"
#include "Tags/StatsTags.h"

ASdCharacterBase::ASdCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AnimationComp = CreateDefaultSubobject<UGeAnimationComponent>(TEXT("AnimationComp"));
	AbilitySystemComp = CreateDefaultSubobject<UGeAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AttributeSet = CreateDefaultSubobject<UGeAttributeSetBase>(TEXT("AttributeSet"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComponent"));
	WeaponMeshComponent->SetupAttachment(GetMesh());
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	WeaponMeshComponent->bCastDynamicShadow = true;
	WeaponMeshComponent->CastShadow = true;

	LocomotionComp = CreateDefaultSubobject<UGeLocomotionComponent>(TEXT("LocomotionComp"));
}

void ASdCharacterBase::ServerSideInit()
{
	AbilitySystemComp->InitAbilityActorInfo(this, this);
	AbilitySystemComp->ServerSideInit();
}

void ASdCharacterBase::ClientSideInit()
{
	AbilitySystemComp->InitAbilityActorInfo(this, this);
}

bool ASdCharacterBase::IsLocallyControlledByPlayer()
{
	return GetController() && GetController()->IsLocalPlayerController();
}

void ASdCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	GetNetworkDebugInfo();
	
	BindGASChangeDelegates();

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		// 绑层必须等动画图节点初始化完成之后再做，OnAnimInitialized 是引擎提供的正确时机。
		MeshComp->OnAnimInitialized.AddDynamic(this, &ThisClass::HandleAnimInitialized);
	}

	if (LocomotionComp)
	{
		// 动画配置挂在组件上（见 UGeLocomotionComponent::AnimationSetup），
		// 配置被换掉时由组件广播，这里转成"让 AnimInstance 重新绑层"。
		LocomotionComp->OnAnimationSetupChanged.AddUObject(this, &ThisClass::HandleAnimationSetupChanged);
	}

	// AnimInstance 若在 BeginPlay 之前就已创建，OnAnimInitialized 不会再来一次，这里补一次。
	RefreshHeroLocomotionLayers();

	InitCharacterDef();
}

void ASdCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnBindGASChangeDelegates();

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->OnAnimInitialized.RemoveDynamic(this, &ThisClass::HandleAnimInitialized);
	}

	if (LocomotionComp)
	{
		LocomotionComp->OnAnimationSetupChanged.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ASdCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASdCharacterBase, CurrentWeaponSocket);
}

void ASdCharacterBase::InitCharacterDef()
{
	USdGISubsystemCharacter* Subsystem = USdGISubsystemCharacter::Get(this);
	if (!Subsystem) return;

	Subsystem->LoadCharacter(
		CharacterTag,
		FOnCharacterDefinitionLoaded::CreateWeakLambda(
			this,
			[this](const UPA_CharacterDefinition* Definition)
			{
				InitAnimationSet(Definition);
			}
		)
	);
}

UGeAbilitySystemComponent* ASdCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

void ASdCharacterBase::BindGASChangeDelegates()
{
	if (AbilitySystemComp)
	{
		DrawWeaponEventHandle = AbilitySystemComp->AddGameplayEventTagContainerDelegate(
			FGameplayTagContainer(EventTags::Ge_Event_DrawWeapon),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				this, &ThisClass::HandleDrawWeaponEvent
			)
		);

		SheatheWeaponEventHandle = AbilitySystemComp->AddGameplayEventTagContainerDelegate(
			FGameplayTagContainer(EventTags::Ge_Event_SheatheWeapon),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				this, &ThisClass::HandleSheatheWeaponEvent
			)
		);

		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
			UGeAttributeSetBase::GetMoveSpeedAttribute()
		).AddUObject(
			this, &ThisClass::MoveSpeedUpdated
		);
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
			UGeAttributeSetBase::GetFlySpeedAttribute()
		).AddUObject(
			this, &ThisClass::FlySpeedUpdated
		);
	}
}

void ASdCharacterBase::UnBindGASChangeDelegates()
{
	if (IsValid(AbilitySystemComp))
	{
		AbilitySystemComp->RemoveGameplayEventTagContainerDelegate(
			FGameplayTagContainer(EventTags::Ge_Event_DrawWeapon), DrawWeaponEventHandle
		);
		AbilitySystemComp->RemoveGameplayEventTagContainerDelegate(
			FGameplayTagContainer(EventTags::Ge_Event_SheatheWeapon), SheatheWeaponEventHandle
		);
	}
}

void ASdCharacterBase::MoveSpeedUpdated(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void ASdCharacterBase::FlySpeedUpdated(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxFlySpeed = Data.NewValue;
}

void ASdCharacterBase::HandleDrawWeaponEvent(FGameplayTag GameplayTag, const FGameplayEventData* GameplayEventData)
{
	// TODO: 后面这一块放到EquipmentComp里
	if(const USocketNameWrapper* Wrapper = Cast<USocketNameWrapper>(GameplayEventData->OptionalObject))
	{
		// 走"请求"，由服务器确认后再复制给所有端：
		// 远端客户端可能根本没播到这条 montage（收不到通知），靠复制值兜底才不会再错位
		RequestWeaponSocket(Wrapper->SocketName);
	}
}

void ASdCharacterBase::HandleSheatheWeaponEvent(FGameplayTag GameplayTag, const FGameplayEventData* GameplayEventData)
{
	if(const USocketNameWrapper* Wrapper = Cast<USocketNameWrapper>(GameplayEventData->OptionalObject))
	{
		RequestWeaponSocket(Wrapper->SocketName);
	}
}

void ASdCharacterBase::DrawWeapon(FName InSocketName)
{
	// 只改本地表现，不写复制值、不发 RPC（给 BeginPlay 之类的本地兜底用）
	ApplyWeaponSocket(InSocketName);
}

void ASdCharacterBase::SheatheWeapon(FName InSocketName)
{
	ApplyWeaponSocket(InSocketName);
}

void ASdCharacterBase::RequestWeaponSocket(FName InSocketName)
{
	if (InSocketName.IsNone())
	{
		return;
	}

	if (HasAuthority())
	{
		// 服务器：更新复制值（authority 上不会触发 OnRep，所以本地表现要自己做）
		CurrentWeaponSocket = InSocketName;
		ApplyWeaponSocket(InSocketName);
		return;
	}

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		// 本地立刻生效（手感），再请服务器确认并广播
		ApplyWeaponSocket(InSocketName);
		ServerSetWeaponSocket(InSocketName);
		return;
	}

	// SimulatedProxy：不响应本地的 montage的 GameplayEvent 通知。
	// 所以SimulatedProxy端的角色武器位置只认服务器的复制值（OnRep_CurrentWeaponSocket）。
}

void ASdCharacterBase::ApplyWeaponSocket(FName InSocketName)
{
	if (InSocketName.IsNone() || AppliedWeaponSocket == InSocketName || !WeaponMeshComponent)
	{
		return;
	}

	if (WeaponMeshComponent->GetSkeletalMeshAsset() == nullptr)
	{
		// 武器 Mesh 是软引用，每个客户端各自异步加载。
		// 加载没完成时不能把请求丢掉（旧代码 SheatheWeapon 就是直接 return），
		// 否则这次收剑就永久丢失，等 Mesh 加载完武器会停在旧插槽上。
		PendingWeaponSocket = InSocketName;
		RequestWeaponMeshLoad();
		return;
	}

	AppliedWeaponSocket = InSocketName;
	WeaponMeshComponent->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		InSocketName
	);
}

void ASdCharacterBase::OnRep_CurrentWeaponSocket()
{
	ApplyWeaponSocket(CurrentWeaponSocket);
}

void ASdCharacterBase::ServerSetWeaponSocket_Implementation(FName InSocketName)
{
	if (CurrentWeaponSocket == InSocketName)
	{
		return;
	}

	CurrentWeaponSocket = InSocketName;
	ApplyWeaponSocket(InSocketName);
}

void ASdCharacterBase::RequestWeaponMeshLoad()
{
	if (bWeaponMeshLoadRequested || DefaultWeaponMesh.IsNull())
	{
		return;
	}

	if (DefaultWeaponMesh.IsValid())
	{
		OnWeaponMeshLoaded();
		return;
	}

	bWeaponMeshLoadRequested = true;

	TWeakObjectPtr<ASdCharacterBase> WeakThis(this);
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		DefaultWeaponMesh.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([WeakThis]()
		{
			if (ASdCharacterBase* Character = WeakThis.Get())
			{
				Character->OnWeaponMeshLoaded();
			}
		})
	);
}

void ASdCharacterBase::OnWeaponMeshLoaded()
{
	bWeaponMeshLoadRequested = false;

	USkeletalMesh* LoadedMesh = DefaultWeaponMesh.Get();
	if (!LoadedMesh || !WeaponMeshComponent)
	{
		return;
	}

	WeaponMeshComponent->SetSkeletalMesh(LoadedMesh);

	// 换了 Mesh，之前贴的插槽作废，必须重新贴一次
	AppliedWeaponSocket = NAME_None;
	const FName SocketToApply = PendingWeaponSocket.IsNone() ? CurrentWeaponSocket : PendingWeaponSocket;
	PendingWeaponSocket = NAME_None;

	ApplyWeaponSocket(SocketToApply);
}

void ASdCharacterBase::HandleAnimInitialized()
{
	// 此刻动画图节点（包括 Linked Anim Layer 节点）已经初始化完成，
	// 是 LinkAnimClassLayers 的安全时机。
	RefreshHeroLocomotionLayers();
}

void ASdCharacterBase::HandleAnimationSetupChanged()
{
	// 运行时换了动画配置：层实现类可能变了，重新绑一次。
	RefreshHeroLocomotionLayers();
}

void ASdCharacterBase::RefreshHeroLocomotionLayers()
{
	USdAnimInstanceHeroBase* HeroAnimInstance = GetMesh()
		? Cast<USdAnimInstanceHeroBase>(GetMesh()->GetAnimInstance())
		: nullptr;

	if (HeroAnimInstance)
	{
		HeroAnimInstance->RefreshLocomotionLayers();
	}
}

void ASdCharacterBase::InitAnimationSet(const UPA_CharacterDefinition* InDefinition)
{
	if (!InDefinition) return;

	if (AnimationComp)
	{
		AnimationComp->SetAnimationSet(InDefinition->LoadAnimationSet());
	}

	if (AbilitySystemComp)
	{
		for (TPair<FGameplayTag, FCharacterMontageEntry> Pair : InDefinition->LoadAnimationSet()->Montages)
		{
			AbilitySystemComp->UpdateMontageMap(Pair.Key, Pair.Value.Montage);
		}

		AbilitySystemComp->PreLoadMontages();
	}
}

void ASdCharacterBase::GetNetworkDebugInfo() const
{
	if (!bDebugNetworkInfo) return;

	// 1. NetMode (当前运行模式)
	FString NetModeStr;
	switch (GetNetMode())
	{
	case NM_Standalone: NetModeStr = TEXT("Standalone");
		break;
	case NM_DedicatedServer: NetModeStr = TEXT("DedicatedServer");
		break;
	case NM_ListenServer: NetModeStr = TEXT("ListenServer");
		break;
	case NM_Client: NetModeStr = TEXT("Client");
		break;
	default: NetModeStr = TEXT("Unknown");
		break;
	}

	// 2. NetRole (本端的角色)
	FString RoleStr;
	switch (GetLocalRole())
	{
	case ROLE_None: RoleStr = TEXT("None");
		break;
	case ROLE_SimulatedProxy: RoleStr = TEXT("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy: RoleStr = TEXT("AutonomousProxy");
		break;
	case ROLE_Authority: RoleStr = TEXT("Authority");
		break;
	default: RoleStr = TEXT("Unknown");
		break;
	}

	// 3. RemoteRole (对端的角色)
	FString RemoteRoleStr;
	switch (GetRemoteRole())
	{
	case ROLE_None: RemoteRoleStr = TEXT("None");
		break;
	case ROLE_SimulatedProxy: RemoteRoleStr = TEXT("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy: RemoteRoleStr = TEXT("AutonomousProxy");
		break;
	case ROLE_Authority: RemoteRoleStr = TEXT("Authority");
		break;
	default: RemoteRoleStr = TEXT("Unknown");
		break;
	}

	// 4. 关键判断
	bool bHasAuthority = HasAuthority();
	bool bIsLocallyControlled = IsLocallyControlled();
	bool bIsServer = (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer);
	bool bIsClientOnly = (GetNetMode() == NM_Client);
	bool bIsStandalone = (GetNetMode() == NM_Standalone);

	// 组装信息
	const FString Info = FString::Printf(
		TEXT("=== %s ===\n")
		TEXT("NetMode: %s\n")
		TEXT("LocalRole: %s | RemoteRole: %s\n")
		TEXT("HasAuthority: %s\n")
		TEXT("IsLocallyControlled: %s\n")
		TEXT("IsServer: %s | IsClientOnly: %s | IsStandalone: %s\n")
		TEXT("Controller: %s\n")
		TEXT("PlayerState: %s"),
		*GetName(),
		*NetModeStr,
		*RoleStr,
		*RemoteRoleStr,
		bHasAuthority ? TEXT("true") : TEXT("false"),
		bIsLocallyControlled ? TEXT("true") : TEXT("false"),
		bIsServer ? TEXT("true") : TEXT("false"),
		bIsClientOnly ? TEXT("true") : TEXT("false"),
		bIsStandalone ? TEXT("true") : TEXT("false"),
		GetController() ? *GetController()->GetName() : TEXT("NULL"),
		GetPlayerState() ? *GetPlayerState()->GetPlayerName() : TEXT("NULL")
	);

	Debug::Print(Info, "NetworkDebug", 30.f);
}

/**********************************************************************/
/*                              Landing                               */
/**********************************************************************/

void ASdCharacterBase::MoveBlockedBy(const FHitResult& Impact)
{
	Super::MoveBlockedBy(Impact);

	// 只处理真正“顶到地面”的情况：
	// 1) 命中面得是可走地面（撞墙、撞斜面不算落地）
	if (!Impact.IsValidBlockingHit() || Impact.ImpactNormal.Z < 0.7f)
	{
		return;
	}

	// 2) 只有自己算得准的端才发事件：服务器（move replay）和本地控制端。
	//    模拟端发了也没意义，反而可能出现“只有本机播了落地”的错位。
	if (!HasAuthority() && !IsLocallyControlled())
	{
		return;
	}

	UGeAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	// 3) 必须是飞行状态；跳跃落地那种走别的流程
	if (!ASC->HasMatchingGameplayTag(StatsTags::Ge_Stats_Fly))
	{
		return;
	}

	// 4) 已经在落地中 / 还在冷却里 → 忽略（贴着地面时每个 tick 都会来一次 blocking hit）
	if (ASC->HasMatchingGameplayTag(StatsTags::Ge_Stats_Landing))
	{
		return;
	}

	// 5) 正在闪避就等它结束：闪避的位移任务还在推，这时候插落地蒙太奇会打架。
	//    闪避结束后如果还压着地面，下一个 tick 的 blocking hit 会再来一次。
	if (ASC->HasMatchingGameplayTag(StatsTags::Ge_Stats_Dodging))
	{
		return;
	}

	// 6) 下坠速度不够 → 当“贴地低飞”，不算落地
	const float FallSpeed = -GetVelocity().Z;
	if (FallSpeed < MinLandFallSpeed)
	{
		return;
	}

	// 7) 冷却
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (LastLandEventTime >= 0.f && (Now - LastLandEventTime) < LandEventCooldown)
	{
		return;
	}

	NotifyLandingEvent(FallSpeed);
}

void ASdCharacterBase::NotifyLandingEvent(float FallSpeed)
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	LastLandEventTime = Now;

	FGameplayEventData EventData;
	EventData.EventTag = EventTags::Ge_Event_FlyLand;
	EventData.Instigator = this;
	EventData.Target = this;
	// 用 EventMagnitude 把撞击瞬间的下坠速度带过去：GA_Land 靠它分硬 / 软落地。
	// （碰撞后 Velocity.Z 会被滑移修正掉，能力里再读就来不及了。）
	EventData.EventMagnitude = FallSpeed;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTags::Ge_Event_FlyLand, EventData);

	Debug::Print(
		FString::Printf(TEXT("[SdCharacter] 撞地：下坠速度=%.1f 发 Ge.Event.Land"), FallSpeed),
		TEXT("Land.Event"), 3.f, FColor::Orange);
}

