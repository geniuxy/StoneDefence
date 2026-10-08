// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Characters/SdCharacterBase.h"
#include "Datas/PrimaryDataAssets/PA_CharacterDefinition.h"
#include "Interfaces/FaceSculpting.h"
#include "SdCharacterHeroBase.generated.h"

enum class EAbilityInputID : uint8;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class STONEDEFENCE_API ASdCharacterHeroBase : public ASdCharacterBase, public IFaceSculpting
{
	GENERATED_BODY()

public:
	ASdCharacterHeroBase();

	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	/**********************************************************************/
	/*                           Camera View                              */
	/**********************************************************************/
private:
	UPROPERTY(VisibleDefaultsOnly, Category="View")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category="View")
	UCameraComponent* ViewCamera;
	
	/**********************************************************************/
	/*                              Input                                 */
	/**********************************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* MoveInputAction;

	// UPROPERTY(EditDefaultsOnly, Category="Input")
	// TMap<ECAbilityInputID, UInputAction*> GameplayAbilityInputActions;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* GameplayInputMappingContext;

	void HandleLookInput(const FInputActionValue& InputActionValue);
	void HandleMoveInput(const FInputActionValue& InputActionValue);
	void HandleAbilityInput(const FInputActionValue& InputActionValue, EAbilityInputID InputID);
};
