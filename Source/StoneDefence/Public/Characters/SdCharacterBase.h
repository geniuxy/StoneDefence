// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
