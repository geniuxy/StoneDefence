// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerControllers/Gameplay/SdPlayerControllerGameBase.h"

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
