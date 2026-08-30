// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerControllers/SdPlayerControllerLobby.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/Lobby/SdWidgetLobbyMain.h"

ASdPlayerControllerLobby::ASdPlayerControllerLobby()
{
	bShowMouseCursor = true;

	static ConstructorHelpers::FClassFinder<USdWidgetLobbyMain> LobbyMain_BPClass(
		TEXT("/Game/_Blueprints/Widgets/Lobby/BP_Widget_LobbyMain")
	);
	LobbyMainClass = LobbyMain_BPClass.Class;
}

void ASdPlayerControllerLobby::BeginPlay()
{
	Super::BeginPlay();
	
	if (LobbyMainClass)
	{
		LobbyMain = CreateWidget<USdWidgetLobbyMain>(this, LobbyMainClass);
		LobbyMain->AddToViewport();
	}
}
