// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GameModes/Gameplay/SdGameModeGameBase.h"

#include "Characters/Hero/SdCharacterHeroBase.h"
#include "Controllers/PlayerControllers/Gameplay/SdPlayerControllerGameBase.h"
#include "Frameworks/PlayerStates/Gameplay/SdPlayerStateGameBase.h"

ASdGameModeGameBase::ASdGameModeGameBase()
{
	PlayerControllerClass = ASdPlayerControllerGameBase::StaticClass();
	PlayerStateClass = ASdPlayerStateGameBase::StaticClass();
	
	static ConstructorHelpers::FClassFinder<ASdCharacterHeroBase> PlayerPawnBPClass(
		TEXT("/Game/_Blueprints/Characters/BP_Character_Base")
	);
	if (PlayerPawnBPClass.Class)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
