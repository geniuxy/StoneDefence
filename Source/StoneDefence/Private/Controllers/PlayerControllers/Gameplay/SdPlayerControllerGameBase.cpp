// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerControllers/Gameplay/SdPlayerControllerGameBase.h"

#include "Characters/SdCharacterBase.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "Widgets/GameHud/SdWidgetGameHudMain.h"

ASdPlayerControllerGameBase::ASdPlayerControllerGameBase()
{
	bShowMouseCursor = false;

	static ConstructorHelpers::FClassFinder<USdWidgetGameHudMain> GameHudMain_BPClass(
		TEXT("/Game/_Blueprints/Widgets/GameHud/BP_Widget_GameHudMain")
	);
	GameHudMainClass = GameHudMain_BPClass.Class;
}

void ASdPlayerControllerGameBase::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	if (GameHudMainClass)
	{
		GameHudMain = CreateWidget<USdWidgetGameHudMain>(this, GameHudMainClass);
		if (GameHudMain)
		{
			GameHudMain->AddToViewport();
		}
	}
}

void ASdPlayerControllerGameBase::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	OwningPlayerCharacter = Cast<ASdCharacterBase>(NewPawn);
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->ServerSideInit();
		OwningASC = OwningPlayerCharacter->GetAbilitySystemComponent();
	}
}

void ASdPlayerControllerGameBase::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);

	OwningPlayerCharacter = Cast<ASdCharacterBase>(NewPawn);
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->ClientSideInit();
		OwningASC = OwningPlayerCharacter->GetAbilitySystemComponent();
	}
}
