// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GameModes/SdGameModeLobby.h"

#include "Controllers/PlayerControllers/SdPlayerControllerLobby.h"
#include "Frameworks/PlayerStates/SdPlayerStateLobby.h"
#include "Pawns/SdPawnLobby.h"

ASdGameModeLobby::ASdGameModeLobby()
{
	PlayerControllerClass = ASdPlayerControllerLobby::StaticClass();
	DefaultPawnClass = ASdPawnLobby::StaticClass();
	PlayerStateClass = ASdPlayerStateLobby::StaticClass();
}
