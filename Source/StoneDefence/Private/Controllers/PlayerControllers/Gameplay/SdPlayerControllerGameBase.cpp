// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerControllers/Gameplay/SdPlayerControllerGameBase.h"

#include "Characters/SdCharacterBase.h"
#include "Characters/Hero/SdCharacterHeroBase.h"
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

		// 每次本地玩家拿到新 Pawn，都重新同步一次捏脸数据。
		//
		// 换人（UGA_SwitchCharacter）是"销毁旧角色 + 生成新角色"，
		// 而捏脸数据存在角色实例自己的 IFaceSculpting::FigureSizeMap 里，
		// 新实例是空的，必须重新走一次 Server 同步流程，
		// 否则新角色的身材数据全是 0。
		if (ASdCharacterHeroBase* NewHero = Cast<ASdCharacterHeroBase>(OwningPlayerCharacter))
		{
			NewHero->CallServerUpdateFaceSculpting();
		}
	}
}
