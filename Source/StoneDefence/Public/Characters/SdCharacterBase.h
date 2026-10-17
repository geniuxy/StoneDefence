// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/Character.h"
#include "Types/GeniuxyCommonMacro.h"
#include "SdCharacterBase.generated.h"

class UPA_CharacterDefinition;
struct FGameplayTag;
class UGeAnimationComponent;
class UGeAbilitySystemComponent;

UCLASS()
class STONEDEFENCE_API ASdCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASdCharacterBase();

	void ServerSideInit();
	void ClientSideInit();

	bool IsLocallyControlledByPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, meta=(Categories="Sd.Character"))
	FGameplayTag CharacterTag;

	void InitCharacterDef();

public:
	DATA_ACCESSOR(FGameplayTag, CharacterTag)

	/**********************************************************************/
    /*                               GAS                                  */
    /**********************************************************************/
public:
	UGeAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
	UPROPERTY(VisibleAnywhere)
	UGeAbilitySystemComponent* AbilitySystemComp;

	virtual void BindGASChangeDelegates();
	virtual void UnBindGASChangeDelegates();

	/**********************************************************************/
	/*                            Equipment                               */
	/**********************************************************************/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	// 当前武器所在的插槽，便于判断当前是拔出还是收刀状态
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeaponSocket, BlueprintReadOnly)
	FName CurrentWeaponSocket = NAME_None;

	// 出生时的武器默认插槽（挂在背上）
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	FName DefaultWeaponSocket = FName(TEXT("WeaponBackSocket"));

protected:
	FDelegateHandle DrawWeaponEventHandle;
	void HandleDrawWeaponEvent(FGameplayTag GameplayTag, const FGameplayEventData* GameplayEventData);
	FDelegateHandle SheatheWeaponEventHandle;
	void HandleSheatheWeaponEvent(FGameplayTag GameplayTag, const FGameplayEventData* GameplayEventData);

	/** 只改本地表现（不写复制值、不发 RPC），给 BeginPlay 之类的本地兜底逻辑用 */
	void DrawWeapon(FName InSocketName);
	void SheatheWeapon(FName InSocketName);

	/**
	 * 请求把武器放到指定插槽。
	 * - 服务器：写复制值 + 本地表现；
	 * - 自己的客户端（AutonomousProxy）：本地先应用，再请服务器确认；
	 * - 别人的角色（SimulatedProxy）：忽略本地 montage 通知，只认复制值。
	 */
	void RequestWeaponSocket(FName InSocketName);

	/** 真正执行 Attach；武器 Mesh 还没加载好就先记下来，加载完再贴 */
	void ApplyWeaponSocket(FName InSocketName);

	UFUNCTION()
	void OnRep_CurrentWeaponSocket();

	/** 客户端 → 服务器：请求把武器换到该插槽，服务器改复制值后广播给所有端 */
	UFUNCTION(Server, Reliable)
	void ServerSetWeaponSocket(FName InSocketName);

	/** 武器 Mesh 是软引用，每个客户端各加载一次；加载完成后补一次 Attach */
	void RequestWeaponMeshLoad();
	void OnWeaponMeshLoaded();

	/** 等待 Mesh 加载完成后要贴的插槽 */
	FName PendingWeaponSocket = NAME_None;

	/** 当前真正贴着的插槽，用来避免重复 Attach */
	FName AppliedWeaponSocket = NAME_None;

	bool bWeaponMeshLoadRequested = false;

private:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> DefaultWeaponMesh;

	/**********************************************************************/
	/*                            Animation                               */
	/**********************************************************************/
protected:
	UPROPERTY(VisibleAnywhere)
	UGeAnimationComponent* AnimationComp;

	void InitAnimationSet(const UPA_CharacterDefinition* InDefinition);

	/**********************************************************************/
	/*                             Network                                */
	/**********************************************************************/
protected:
	UPROPERTY(EditDefaultsOnly, Category="Net")
	bool bDebugNetworkInfo = false;

	void GetNetworkDebugInfo() const;
};
