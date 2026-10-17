// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SdCharacterBase.h"

#include "Comps/GeAbilitySystemComponent.h"
#include "Comps/GeAnimationComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/PlayerState.h"
#include "GeniuxyDebugHelper.h"
#include "Animations/AnimNotifies/AN_DrawWeapon.h"
#include "Data/CharacterAnimationSet.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "Net/UnrealNetwork.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemCharacter.h"
#include "Tags/AMEventTags.h"
#include "Tags/CharacterTags.h"

ASdCharacterBase::ASdCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AnimationComp = CreateDefaultSubobject<UGeAnimationComponent>(TEXT("AnimationComp"));
	AbilitySystemComp = CreateDefaultSubobject<UGeAbilitySystemComponent>(TEXT("AbilitySystemComp"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComponent"));
	WeaponMeshComponent->SetupAttachment(GetMesh());
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	WeaponMeshComponent->bCastDynamicShadow = true;
	WeaponMeshComponent->CastShadow = true;
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

	InitCharacterDef();
}

void ASdCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UnBindGASChangeDelegates();
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
			FGameplayTagContainer(AMEventTags::Ge_Event_DrawWeapon),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				this, &ThisClass::HandleDrawWeaponEvent
			)
		);

		SheatheWeaponEventHandle = AbilitySystemComp->AddGameplayEventTagContainerDelegate(
			FGameplayTagContainer(AMEventTags::Ge_Event_SheatheWeapon),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				this, &ThisClass::HandleSheatheWeaponEvent
			)
		);
	}
}

void ASdCharacterBase::UnBindGASChangeDelegates()
{
	if (IsValid(AbilitySystemComp))
	{
		AbilitySystemComp->RemoveGameplayEventTagContainerDelegate(
			FGameplayTagContainer(AMEventTags::Ge_Event_DrawWeapon), DrawWeaponEventHandle
		);
		AbilitySystemComp->RemoveGameplayEventTagContainerDelegate(
			FGameplayTagContainer(AMEventTags::Ge_Event_SheatheWeapon), SheatheWeaponEventHandle
		);
	}
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

	Debug::Print(Info, -1, 30.f);
}

