// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Hero/SdCharacterHeroBase.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GeniuxyGASType.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


ASdCharacterHeroBase::ASdCharacterHeroBase()
{
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 250.f;
	CameraBoom->SocketOffset = FVector(0.f, 80.f, 50.f);

	ViewCamera = CreateDefaultSubobject<UCameraComponent>("View Camera");
	ViewCamera->SetupAttachment(CameraBoom);
	ViewCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);
}

void ASdCharacterHeroBase::PawnClientRestart()
{
	Super::PawnClientRestart();

	APlayerController* OwningPlayerController = GetController<APlayerController>();
	if (OwningPlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem)
		{
			InputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			InputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);
		}
	}
}

void ASdCharacterHeroBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(
			JumpInputAction, ETriggerEvent::Triggered, this, &ThisClass::Jump
		);
		EnhancedInputComponent->BindAction(
			LookInputAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput
		);
		EnhancedInputComponent->BindAction(
			MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput
		);

		for (const TPair<EAbilityInputID, UInputAction*>& InputActionPair :
		     AbilitySystemComp->GetGameplayAbilityInputActions())
		{
			EnhancedInputComponent->BindAction(
				InputActionPair.Value, ETriggerEvent::Triggered, this,
				&ThisClass::HandleAbilityInput, InputActionPair.Key
			);
		}
	}
}

void ASdCharacterHeroBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASdCharacterHeroBase::HandleLookInput(const FInputActionValue& InputActionValue)
{
	FVector2D LookInputVector = InputActionValue.Get<FVector2D>();

	if (LookInputVector.Y != 0.f)
	{
		AddControllerPitchInput(LookInputVector.Y);
	}
	if (LookInputVector.X != 0.f)
	{
		AddControllerYawInput(LookInputVector.X);
	}
}

void ASdCharacterHeroBase::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D MoveInputVector = InputActionValue.Get<FVector2D>();

	const FRotator ControllerRotation(0.f, GetControlRotation().Yaw, 0.f);

	if (MoveInputVector.Y != 0.f)
	{
		FVector TargetForwardDirection = ControllerRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(TargetForwardDirection, MoveInputVector.Y);
	}

	if (MoveInputVector.X != 0.f)
	{
		FVector TargetRightDirection = ControllerRotation.RotateVector(FVector::RightVector);
		AddMovementInput(TargetRightDirection, MoveInputVector.X);
	}
}

void ASdCharacterHeroBase::HandleAbilityInput(const FInputActionValue& InputActionValue, EAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();

	// if (bPressed && bIsLearnAbilityLeaderPressedDown)
	// {
	// 	UpgradeAbilityWithInputID(InputID);
	// 	return;
	// }

	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)InputID);
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)InputID);
	}
}


void ASdCharacterHeroBase::UpdateFigureTypeSize(ESdFigureType InType, int32 InValue)
{
	if (FigureSizeMap.Contains(InType))
	{
		FigureSizeMap[InType] = InValue;
	}
	else
	{
		FigureSizeMap.Add(InType, InValue);
	}
}

void ASdCharacterHeroBase::UpdateFigureTypeSize(TArray<FFaceSculptFigureTypeInfo> InFigureSettings)
{
	for (const FFaceSculptFigureTypeInfo& TypeInfo : InFigureSettings)
	{
		UpdateFigureTypeSize(TypeInfo.Type, TypeInfo.CurValue);
	}
}

void ASdCharacterHeroBase::UpdateFigureTypeSizeByDefault(TArray<FFaceSculptFigureTypeInfo> InFigureSettings)
{
	for (const FFaceSculptFigureTypeInfo& TypeInfo : InFigureSettings)
	{
		UpdateFigureTypeSize(
			TypeInfo.Type,
			FMath::Clamp(TypeInfo.DefaultValue * TypeInfo.MaxValue, TypeInfo.MinValue, TypeInfo.MaxValue)
		);
	}
}

void ASdCharacterHeroBase::UpdateFigureTypeSize(const FString& InFigureSizeStr)
{
	TArray<FString> StrFigureSizeArray;
	InFigureSizeStr.ParseIntoArray(StrFigureSizeArray, TEXT("|"));
	for (const FString& StrFigureSize : StrFigureSizeArray)
	{
		TArray<FString> StrFigureTypeAndSize;
		StrFigureSize.ParseIntoArray(StrFigureTypeAndSize, TEXT(","));
		if (StrFigureTypeAndSize.Num() != 2) continue;

		int TypeIndex = FCString::Atoi(*StrFigureTypeAndSize[0]);
		if (TypeIndex >= 0 && TypeIndex < static_cast<int>(ESdFigureType::FT_NUM))
		{
			UpdateFigureTypeSize(static_cast<ESdFigureType>(TypeIndex), FCString::Atoi(*StrFigureTypeAndSize[1]));
		}
	}
}

int32 ASdCharacterHeroBase::GetFigureSizeByType(ESdFigureType InType)
{
	// return FigureSizeMap.Contains(InType) ? FigureSizeMap[InType] : 0;
	return FigureSizeMap.FindRef(InType, 0);
}

