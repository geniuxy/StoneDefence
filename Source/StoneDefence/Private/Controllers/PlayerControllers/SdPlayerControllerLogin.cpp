// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerControllers/SdPlayerControllerLogin.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/Login/SdWidgetLoginMenu.h"

ASdPlayerControllerLogin::ASdPlayerControllerLogin()
{
	bShowMouseCursor = true;

	static ConstructorHelpers::FClassFinder<USdWidgetLoginMenu> LoginMenu_BPClass(
		TEXT("/Game/_Blueprints/Widgets/Login/BP_LoginMenu")
	);
	LoginMenuClass = LoginMenu_BPClass.Class;
}

void ASdPlayerControllerLogin::BeginPlay()
{
	Super::BeginPlay();

	if (LoginMenuClass)
	{
		LoginMenu = CreateWidget<USdWidgetLoginMenu>(this, LoginMenuClass);
		LoginMenu->AddToViewport();
	}
}
