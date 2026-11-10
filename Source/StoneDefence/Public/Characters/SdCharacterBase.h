// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/Character.h"
#include "Types/GeniuxyCommonMacro.h"
#include "SdCharacterBase.generated.h"

class UGeAttributeSetBase;
class UGeLocomotionComponent;
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

	UPROPERTY(VisibleAnywhere, Category= "GAS")
	TObjectPtr<UGeAttributeSetBase> AttributeSet;

	virtual void BindGASChangeDelegates();
	virtual void UnBindGASChangeDelegates();

	void MoveSpeedUpdated(const FOnAttributeChangeData& Data);
	void FlySpeedUpdated(const FOnAttributeChangeData& Data);

	/**********************************************************************/
	/*                            Locomotion                              */
	/**********************************************************************/
protected:
	/**
	 * 移动模式（Ground/Fly/Swim/Climb）+ 移动动画集合的唯一数据源。
	 * AnimInstance（主实例和所有 layer 实例）都从这里读，不要再在 AnimInstance 里注册 GAS tag。
	 */
	UPROPERTY(VisibleAnywhere)
	UGeLocomotionComponent* LocomotionComp;

public:
	FORCEINLINE UGeLocomotionComponent* GetLocomotionComponent() const { return LocomotionComp; }

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

	/**
	 * AnimInstance 初始化完成的回调（USkeletalMeshComponent::OnAnimInitialized）。
	 *
	 * 绑层必须放在这里而不是 UAnimInstance::NativeInitializeAnimation：
	 * 后者执行时动画图节点还没初始化，引擎会告警 "Performing linked layer operations
	 * in initialization may produce unexpected results"。
	 */
	UFUNCTION()
	void HandleAnimInitialized();

	/** UGeLocomotionComponent::OnAnimationSetupChanged 的回调（非动态委托，无需 UFUNCTION） */
	void HandleAnimationSetupChanged();

	/**
	 * 让当前 AnimInstance 按最新配置重新 LinkAnimClassLayers。
	 *
	 * 注意 UGeAnimationSetup 本身不在角色上，而在 UGeLocomotionComponent 上：
	 * 它和「移动模式」属于同一类数据，AnimInstance 本来就从组件读，不用多绕一层角色。
	 */
	void RefreshHeroLocomotionLayers();

	/**********************************************************************/
	/*                             Network                                */
	/**********************************************************************/
protected:
	UPROPERTY(EditDefaultsOnly, Category="Net")
	bool bDebugNetworkInfo = false;

	void GetNetworkDebugInfo() const;

	/**********************************************************************/
	/*                              Landing                               */
	/**********************************************************************/
protected:
	/**
	 * 撞到东西时的回调（CMC 在 HandleImpact 里调用）。
	 *
	 * 这里只负责「报告撞地」：判定是可走地面 + 正在飞行 + 下坠速度够 → 发 Ge.Event.Land，
	 * 具体播硬落地还是软落地、以及退出飞行，交给 GA_Land / GA_Fly 那套能力处理。
	 *
	 * 为什么用它而不是每帧 trace：blocking hit 只在真的顶到地面时才有，
	 * 平飞擦着地面不会触发 —— 正好满足「贴地低飞不算落地」。
	 */
	virtual void MoveBlockedBy(const FHitResult& Impact) override;

	/** 落地候选的最小下坠速度：低于它当「贴地低飞」，直接忽略 */
	UPROPERTY(EditDefaultsOnly, Category="Landing", meta=(ClampMin="0"))
	float MinLandFallSpeed = 250.f;

	/** 两次落地事件的最小间隔，防止贴着地面时每个 tick 都发一次 */
	UPROPERTY(EditDefaultsOnly, Category="Landing", meta=(ClampMin="0"))
	float LandEventCooldown = 0.35f;

	/** 发落地事件：EventMagnitude = 撞击瞬间的下坠速度 */
	void NotifyLandingEvent(float FallSpeed);

	/** 上一次发落地事件的世界时间 */
	float LastLandEventTime = -1.f;
};
