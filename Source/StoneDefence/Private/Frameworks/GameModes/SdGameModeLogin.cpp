// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GameModes/SdGameModeLogin.h"

#include "Controllers/PlayerControllers/SdPlayerControllerLogin.h"
#include "Pawns/SdPawnLogin.h"

ASdGameModeLogin::ASdGameModeLogin()
{
	PlayerControllerClass = ASdPlayerControllerLogin::StaticClass();
	DefaultPawnClass = ASdPawnLogin::StaticClass();
}
