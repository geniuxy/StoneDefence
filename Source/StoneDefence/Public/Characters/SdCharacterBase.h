// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/Character.h"
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

	UPROPERTY(EditDefaultsOnly, meta=(Categories="Sd.Character"))
	FGameplayTag CharacterTag;

	void InitCharacterDef();

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
	UPROPERTY(BlueprintReadOnly)
	FName CurrentWeaponSocket = NAME_None;

protected:
	FDelegateHandle DrawWeaponEventHandle;
	void HandleDrawWeaponEvent(FGameplayTag GameplayTag, const FGameplayEventData* GameplayEventData);
	FDelegateHandle SheatheWeaponEventHandle;
	void HandleSheatheWeaponEvent(FGameplayTag GameplayTag, const FGameplayEventData* GameplayEventData);

	void DrawWeapon(FName InSocketName);
	void SheatheWeapon(FName InSocketName);

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
