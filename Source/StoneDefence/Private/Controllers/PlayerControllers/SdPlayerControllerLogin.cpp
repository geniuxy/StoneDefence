// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerControllers/SdPlayerControllerLogin.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/Login/SdWidgetLoginMain.h"

ASdPlayerControllerLogin::ASdPlayerControllerLogin()
{
	bShowMouseCursor = true;

	static ConstructorHelpers::FClassFinder<USdWidgetLoginMain> LoginMain_BPClass(
		TEXT("/Game/_Blueprints/Widgets/Login/BP_Widget_LoginMain")
	);
	LoginMainClass = LoginMain_BPClass.Class;
}

void ASdPlayerControllerLogin::BeginPlay()
{
	Super::BeginPlay();

	if (LoginMainClass)
	{
		LoginMain = CreateWidget<USdWidgetLoginMain>(this, LoginMainClass);
		LoginMain->AddToViewport();
	}
}
