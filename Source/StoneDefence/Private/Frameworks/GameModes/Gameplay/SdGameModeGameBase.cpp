// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GameModes/Gameplay/SdGameModeGameBase.h"

#include "GeniuxyCommonBPLibrary.h"
#include "Channel/SimpleChannel.h"
#include "Characters/Hero/SdCharacterHeroBase.h"
#include "Controllers/PlayerControllers/Gameplay/SdPlayerControllerGameBase.h"
#include "Frameworks/GameInstance/SdGameInstance.h"
#include "Frameworks/PlayerStates/Gameplay/SdPlayerStateGameBase.h"
#include "SdTypes/SdMacros.h"
#include "Protocol/GameProtocol.h"

ASdGameModeGameBase::ASdGameModeGameBase()
{
	bUseSeamlessTravel = true;

	PrimaryActorTick.bCanEverTick = true;
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

void ASdGameModeGameBase::BeginPlay()
{
	Super::BeginPlay();

	LinkServer();
}

void ASdGameModeGameBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		if (ClientGameInstance->GetClient() && ClientGameInstance->GetClient()->GetController())
		{
			ClientGameInstance->GetClient()->GetController()->RecvDelegate.Remove(ClientRecvDelegate);
		}
	}
}

void ASdGameModeGameBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASdGameModeGameBase::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch (ProtocolNumber)
	{
	case SP_UpdateLoginCharacterInfoResponses:
		{
			HandleUpdateLoginCharacterInfoResponses(Channel);
			break;
		}
	default:
		break;
	}
}

void ASdGameModeGameBase::BindClientRcv()
{
	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		if (ClientGameInstance->GetClient() && ClientGameInstance->GetClient()->GetController())
		{
			ClientRecvDelegate = ClientGameInstance->GetClient()->GetController()->RecvDelegate.AddLambda(
				[&](uint32 ProtocolNumber, FSimpleChannel* Channel)
				{
					this->RecvProtocol(ProtocolNumber, Channel);
				}
			);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::BindClientRcv);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::BindClientRcv);
	}
}

void ASdGameModeGameBase::HandleServerLinkInfo(ESimpleNetErrorType InType, const FString& InMsg)
{
}

void ASdGameModeGameBase::LinkServer()
{
	if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>())
	{
		ClientGameInstance->CreateClient();
		if (ClientGameInstance->GetClient())
		{
			ClientGameInstance->GetClient()->NetManageMsgDelegate.BindUObject(this, &ThisClass::HandleServerLinkInfo);

			ClientGameInstance->LinkServer(TEXT("127.0.0.1"), 11231);

			BindClientRcv();
		}
	}
}

void ASdGameModeGameBase::HandleUpdateLoginCharacterInfoResponses(FSimpleChannel* Channel)
{
	int32 UserId = INDEX_NONE;
	FString CAJsonString;
	SIMPLE_PROTOCOLS_RECEIVE(SP_UpdateLoginCharacterInfoResponses, UserId, CAJsonString);
	if (UserId != INDEX_NONE && !CAJsonString.IsEmpty())
	{
		FSdCharacterAppearance CA;
		NetDataAnalysis::StringToCharacterAppearance(CAJsonString, CA);

		UGeniuxyCommonBPLibrary::ServerCallAllPlayer<ASdCharacterHeroBase>(
			GetWorld(), [&](ASdCharacterHeroBase* InCharacter)-> EServerCallType
			{
				if (InCharacter->GetCachedOwnerUserId() == UserId)
				{
					InCharacter->UpdateFigureTypeSize(CA.FigureSizeStr);
					InCharacter->Client_UpdateFaceSculpting(CA.FigureSizeStr);
					return EServerCallType::SC_PROGRESS_COMPLETE;
				}
				return EServerCallType::SC_INPROGRESS;
			}
		);
	}
}

void ASdGameModeGameBase::LoginCharacterUpdateFaceSculptingRequest(int32 InUserId)
{
	SEND_DATA(SP_UpdateLoginCharacterInfoRequests, InUserId)
}
