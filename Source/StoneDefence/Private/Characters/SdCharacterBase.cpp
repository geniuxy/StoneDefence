// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SdCharacterBase.h"

#include "Comps/GeAbilitySystemComponent.h"
#include "Comps/GeAnimationComponent.h"
#include "GameFramework/PlayerState.h"
#include "GeniuxyDebugHelper.h"
#include "Animations/AnimNotifies/AN_DrawWeapon.h"
#include "Data/CharacterAnimationSet.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "Frameworks/SdAssetManager.h"
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

	DrawWeapon(FName(TEXT("WeaponBackSocket")));
}

void ASdCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UnBindGASChangeDelegates();
}

void ASdCharacterBase::InitCharacterDef()
{
	USdGISubsystemCharacter* Subsystem = USdGISubsystemCharacter::Get(this);
	if (!Subsystem) return;

	const FPrimaryAssetId AssetId(
		UPA_CharacterDefinition::GetCharacterDefinitionAssetType(), 
		CharacterTag.GetTagName()
	);
	Subsystem->LoadCharacter(
		AssetId,
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
		FName SocketName = Wrapper->SocketName;
		DrawWeapon(SocketName);
	}
}

void ASdCharacterBase::HandleSheatheWeaponEvent(FGameplayTag GameplayTag, const FGameplayEventData* GameplayEventData)
{
	if(const USocketNameWrapper* Wrapper = Cast<USocketNameWrapper>(GameplayEventData->OptionalObject))
	{
		FName SocketName = Wrapper->SocketName;
		SheatheWeapon(SocketName);
	}
}

void ASdCharacterBase::DrawWeapon(FName InSocketName)
{
	const FSoftObjectPath MeshPath = DefaultWeaponMesh.ToSoftObjectPath();
	TWeakObjectPtr<ASdCharacterBase> WeakThis(this);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(MeshPath, [WeakThis, MeshPath, InSocketName]()
	{
		if (ASdCharacterBase* Character = WeakThis.Get())
		{
			if (USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.ResolveObject()))
			{
				Character->WeaponMeshComponent->SetSkeletalMesh(LoadedMesh);
				Character->WeaponMeshComponent->AttachToComponent(
					Character->GetMesh(),
					FAttachmentTransformRules::SnapToTargetIncludingScale,
					InSocketName
				);
				Character->CurrentWeaponSocket = InSocketName;
			}
		}
	});
}

void ASdCharacterBase::SheatheWeapon(FName InSocketName)
{
	if (!WeaponMeshComponent->GetSkeletalMeshAsset())
	{
		return;
	}

	WeaponMeshComponent->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		InSocketName
	);
	CurrentWeaponSocket = InSocketName;
}

void ASdCharacterBase::InitAnimationSet(const UPA_CharacterDefinition* InDefinition)
{
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

