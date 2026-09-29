// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleAdvancedAnimation.h"

#include "Cores/SimpleAdvancedAnimSystem.h"

#define LOCTEXT_NAMESPACE "FSimpleAdvancedAnimationModule"

void FSimpleAdvancedAnimationModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FSimpleAdvancedAnimationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FSimpleAdvancedAnimSystem::Destroy();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimpleAdvancedAnimationModule, SimpleAdvancedAnimation)