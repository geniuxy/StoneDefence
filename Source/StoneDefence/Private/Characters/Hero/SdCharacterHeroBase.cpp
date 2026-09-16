// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Hero/SdCharacterHeroBase.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
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

