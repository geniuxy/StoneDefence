// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Hero/SdCharacterHeroBase.h"

#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GeniuxyDebugHelper.h"
#include "GeniuxyGASType.h"
#include "AttributeSets/GeAttributeSetHero.h"
#include "SdTypes/SdMacros.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "Comps/Locomotions/GeLocomotionComponent.h"
#include "Types/GeGASPayloads.h"
#include "Frameworks/PlayerStates/Gameplay/SdPlayerStateGameBase.h"
#include "Frameworks/SdAssetManager.h"
#include "Frameworks/GameInstance/SdGameInstance.h"
#include "Frameworks/GameModes/Gameplay/SdGameModeGameBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsytems/SdGISubsystemCharacter.h"
#include "Tags/CharacterTags.h"
#include "Tags/EventTags.h"
#include "Widgets/GameHud/Partners/SdButtonPartnerInfo.h"


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
	GetCharacterMovement()->BrakingDecelerationFlying = 1400.f; // 为了能够在飞行状态，没有输入的情况下，可以迅速停下来

	HeroAttributeSet = CreateDefaultSubobject<UGeAttributeSetHero>("HeroAttributeSet");
}

void ASdCharacterHeroBase::PawnClientRestart()
{
	Super::PawnClientRestart();

	APlayerController* OwningPlayerController = GetController<APlayerController>();
	if (!OwningPlayerController) return;

	ULocalPlayer* LocalPlayer = OwningPlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem) return;

	// 键位方案的唯一真源：UPA_AbilitySystemGenerics::GameplayInputMappingContext
	// （和 GameplayAbilityInputActions 放在一起，改键位和改映射不会各改一半）
	UInputMappingContext* MappingContext = AbilitySystemComp ? AbilitySystemComp->GetInputMappingContext() : nullptr;

	if (!MappingContext)
	{
		Debug::Print(
			TEXT("[SdCharacterHeroBase] 没取到 GameplayInputMappingContext：")
			TEXT("去 PA_AbilitySystemGenerics 的 Input 分类里配（角色上不再单独配）"),
			TEXT("Input.MappingContext"), 8.f, FColor::Red);
		return;
	}

	InputSubsystem->RemoveMappingContext(MappingContext);
	InputSubsystem->AddMappingContext(MappingContext, 0);
}

void ASdCharacterHeroBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(
			JumpInputAction, ETriggerEvent::Triggered, this, &ThisClass::HandleJumpInput
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
				InputActionPair.Value, ETriggerEvent::Started, this,
				&ThisClass::HandleAbilityInput, InputActionPair.Key
			);
			// 释放 / 取消也必须绑，否则 AbilityLocalInputReleased 永远不会被调用，
			// WaitInputRelease 之类的 Task 就永远等不到回调。
			EnhancedInputComponent->BindAction(
				InputActionPair.Value, ETriggerEvent::Completed, this,
				&ThisClass::HandleAbilityInput, InputActionPair.Key
			);
			EnhancedInputComponent->BindAction(
				InputActionPair.Value, ETriggerEvent::Canceled, this,
				&ThisClass::HandleAbilityInput, InputActionPair.Key
			);
		}
	}
}

void ASdCharacterHeroBase::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
#if !SD_ROBOT_TEST
		CallServerUpdateFaceSculpting();
#endif
	}

	if (CharacterTag == CharacterTags::Sd_Character_Tubaki)
	{
		DrawWeapon(FName(TEXT("WeaponBackSocket")));
	}
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

void ASdCharacterHeroBase::HandleJumpInput()
{
	// 游泳里空格 = 上浮：持续往角色加向上的移动输入（和 GA_Swim_Dive 加向下输入对称）。
	// 不走 ACharacter::Jump()：游泳时的 Jump 走的是引擎的「出水跳」逻辑，会把
	// MOVE_Swimming 打断成 MOVE_Falling，人会被直接弹到岸上而不是浮到水面。
	if (const UGeLocomotionComponent* LocoComp = GetLocomotionComponent())
	{
		if (LocoComp->IsInMode(EGeLocomotionMode::Swim))
		{
			AddMovementInput(FVector::UpVector, 1.f);
			return;
		}
	}

	Jump();
}

void ASdCharacterHeroBase::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	const FVector2D MoveInput = InputActionValue.Get<FVector2D>();
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	FVector MoveDirection = FVector::ZeroVector;

	if (GetLocomotionComponent()->IsInMode(EGeLocomotionMode::Fly))
	{
		// 前后沿视线，含 Pitch
		const FVector Forward = GetControlRotation().Vector();

		// 左右保持水平，不受 Roll/Pitch 影响
		const FVector Right = YawRotation.RotateVector(FVector::RightVector);

		MoveDirection = Forward * MoveInput.Y + Right * MoveInput.X;
	}
	else
	{
		MoveDirection =
			YawRotation.RotateVector(FVector::ForwardVector) * MoveInput.Y +
			YawRotation.RotateVector(FVector::RightVector) * MoveInput.X;
	}

	if (!MoveDirection.IsNearlyZero())
	{
		AddMovementInput(MoveDirection.GetClampedToMaxSize(1.f));
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

void ASdCharacterHeroBase::Server_UpdateFaceSculpting_Implementation(int32 InUserId)
{
	if (ASdGameModeGameBase* InGameMode = GetWorld()->GetAuthGameMode<ASdGameModeGameBase>())
	{
		CachedOwnerUserId = InUserId;
		InGameMode->LoginCharacterUpdateFaceSculptingRequest(InUserId);
	}
}

void ASdCharacterHeroBase::CallServerUpdateFaceSculpting()
{
	// 不管走哪条路，先用"默认身材"打底：
	// 后端数据到了会逐类型覆盖，没给的类型就保持默认值，
	// 而不是让 GetFigureSizeByType 兜底返回 0。
	// 注意 UpdateFigureTypeSizeByDefault 算的是 DefaultValue * MaxValue，
	// 所以 PA 里 MaxValue 必须填对，否则算出来还是 0。
	if (const UPA_CharacterDefinition* Definition =
		USdAssetManager::Get().GetCharacterDefinition(CharacterTag.GetTagName()))
	{
		UpdateFigureTypeSizeByDefault(Definition->GetDefaultFigureSettings());
	}

	if (!bNeedFaceSculpt)
	{
		// 不需要捏脸数据的角色，到这就够了
		return;
	}

	if (USdGameInstance* InGameInstance = GetWorld()->GetGameInstance<USdGameInstance>())
	{
#if SD_ROBOT_TEST
		Server_UpdateFaceSculpting(SD_ROBOT_USERID);
#else
		Server_UpdateFaceSculpting(InGameInstance->GetUserData().Id);
#endif
	}
}

void ASdCharacterHeroBase::Client_UpdateFaceSculpting_Implementation(const FString& InFigureString)
{
	UpdateFigureTypeSize(InFigureString);
}

void ASdCharacterHeroBase::BindGASChangeDelegates()
{
	Super::BindGASChangeDelegates();

	if (AbilitySystemComp)
	{
		SwitchCharacterEventHandle = AbilitySystemComp->AddGameplayEventTagContainerDelegate(
			FGameplayTagContainer(EventTags::Ge_Event_SwitchCharacter),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				this, &ThisClass::HandleSwitchCharacterEvent
			)
		);
	}
}

void ASdCharacterHeroBase::UnBindGASChangeDelegates()
{
	if (IsValid(AbilitySystemComp))
	{
		AbilitySystemComp->RemoveGameplayEventTagContainerDelegate(
			FGameplayTagContainer(EventTags::Ge_Event_SwitchCharacter), SwitchCharacterEventHandle
		);
	}

	Super::UnBindGASChangeDelegates();
}

void ASdCharacterHeroBase::HandleSwitchCharacterEvent(
	FGameplayTag GameplayTag, const FGameplayEventData* GameplayEventData)
{
	if (!GameplayEventData) return;

	int32 SlotIndex = INDEX_NONE;

	if (const USwitchCharacterPayload* Payload = Cast<USwitchCharacterPayload>(GameplayEventData->OptionalObject))
	{
		SlotIndex = Payload->SlotIndex;
	}
	else if (GameplayEventData->EventMagnitude != 0.f)
	{
		// 兼容只塞了 EventMagnitude 的调用方
		SlotIndex = FMath::RoundToInt(GameplayEventData->EventMagnitude);
	}

	if (SlotIndex == INDEX_NONE) return;

	RequestSwitchCharacter(SlotIndex);
}

void ASdCharacterHeroBase::RequestSwitchCharacter(int32 InSlotIndex)
{
	if (HasAuthority())
	{
		PerformSwitchCharacter(InSlotIndex);
	}
	else
	{
		Server_SwitchCharacter(InSlotIndex);
	}
}

void ASdCharacterHeroBase::Server_SwitchCharacter_Implementation(int32 InSlotIndex)
{
	PerformSwitchCharacter(InSlotIndex);
}

void ASdCharacterHeroBase::PerformSwitchCharacter(int32 InSlotIndex)
{
	// 换人是服务端权威行为
	if (!HasAuthority()) return;

	// 1. 从 PlayerState 的队伍信息里找到这个槽位对应的角色
	ASdPlayerStateGameBase* GamePlayerState = GetPlayerState<ASdPlayerStateGameBase>();
	if (!GamePlayerState)
	{
		Debug::Print(TEXT("[SwitchCharacter] 拿不到 ASdPlayerStateGameBase"));
		return;
	}

	const UPartnerInfo* TargetPartner = GamePlayerState->GetPartnerInfoBySlotIndex(InSlotIndex);
	if (!TargetPartner)
	{
		Debug::Print(FString::Printf(
			TEXT("[SwitchCharacter] 槽位 %d 没有对应的队友信息（队伍里现在有 %d 个）"),
			InSlotIndex, GamePlayerState->GetPartnerCount()));
		return;
	}

	const FGameplayTag TargetTag = TargetPartner->GetCharacterTag();
	if (!TargetTag.IsValid() || TargetTag == CharacterTag)
	{
		// 已经就是这个角色，或者槽位上的 tag 无效
		return;
	}

	// 2. 拿到目标角色的定义（没加载完就异步加载好之后再切一次）
	UPA_CharacterDefinition* TargetDefinition =
		USdAssetManager::Get().GetCharacterDefinition(TargetTag.GetTagName());

	if (!TargetDefinition)
	{
		USdGISubsystemCharacter* Subsystem = USdGISubsystemCharacter::Get(this);
		if (!Subsystem) return;

		Subsystem->LoadCharacter(
			TargetTag,
			FOnCharacterDefinitionLoaded::CreateWeakLambda(
				this,
				[this, InSlotIndex](const UPA_CharacterDefinition* LoadedDefinition)
				{
					if (LoadedDefinition)
					{
						PerformSwitchCharacter(InSlotIndex);
					}
				}
			)
		);
		return;
	}

	TSubclassOf<ASdCharacterBase> NewCharacterClass = TargetDefinition->LoadCharacterClass();
	if (!NewCharacterClass)
	{
		Debug::Print(FString::Printf(
			TEXT("[SwitchCharacter] %s 没有配置 CharacterClass"), *TargetTag.ToString()));
		return;
	}

	AController* OldController = GetController();
	if (!OldController) return;

	const FTransform SpawnTransform = GetActorTransform();
	const FVector OldVelocity = GetVelocity();

	// 3. Deferred 生成：先把 CharacterTag 填好再 FinishSpawning，
	//    否则 BeginPlay 里的 InitCharacterDef() 会拿着蓝图默认 tag 去查资源
	ASdCharacterBase* NewCharacter = GetWorld()->SpawnActorDeferred<ASdCharacterBase>(
		NewCharacterClass,
		SpawnTransform,
		OldController,
		this,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (!NewCharacter)
	{
		Debug::Print(TEXT("[SwitchCharacter] 生成新角色失败"));
		return;
	}

	NewCharacter->SetCharacterTag(TargetTag);
	UGameplayStatics::FinishSpawningActor(NewCharacter, SpawnTransform);

	// 4. 控制器接管新角色（ASdPlayerControllerGameBase::OnPossess 里会做 ServerSideInit）
	OldController->Possess(NewCharacter);
	NewCharacter->GetCharacterMovement()->Velocity = OldVelocity;

	// 5. 最后销毁自己
	Destroy();
}
