// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"

USdGISubsystemLobby* USdGISubsystemLobby::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

		return UGameInstance::GetSubsystem<USdGISubsystemLobby>(World->GetGameInstance());
	}

	return nullptr;
}
