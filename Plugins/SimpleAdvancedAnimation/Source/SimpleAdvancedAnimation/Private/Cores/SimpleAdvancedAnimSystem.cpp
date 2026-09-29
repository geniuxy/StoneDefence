#include "Cores/SimpleAdvancedAnimSystem.h"

FSimpleAdvancedAnimSystem * FSimpleAdvancedAnimSystem::AdvancedAnimSystem = nullptr;

FSimpleAdvancedAnimSystem* FSimpleAdvancedAnimSystem::Get()
{
	if (!AdvancedAnimSystem)
	{
		AdvancedAnimSystem = new FSimpleAdvancedAnimSystem();
	}

	return AdvancedAnimSystem;
}

void FSimpleAdvancedAnimSystem::Destroy()
{
	if (AdvancedAnimSystem)
	{
		delete AdvancedAnimSystem;
		AdvancedAnimSystem = nullptr;
	}
}

TStatId FSimpleAdvancedAnimSystem::GetStatId() const
{
	return TStatId();
}

void FSimpleAdvancedAnimSystem::Tick(float DeltaTime)
{
	FootIKSystem.Tick(DeltaTime);
}
