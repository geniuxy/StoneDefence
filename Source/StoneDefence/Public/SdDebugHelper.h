#pragma once

namespace Debug
{
	static void Print(
		const FString& Msg, int32 InKey = -1, float Duration = 15.f, const FColor& InColor = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, Msg);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		}
	}
	
	static void PrintWithInt(const FString& Msg, int InValue)
	{
		Print(FString::Printf(TEXT("%s: %d"), *Msg, InValue));
	}
	
	static void Print(const FString& Msg, float InValue)
	{
		Print(FString::Printf(TEXT("%s: %f"), *Msg, InValue));
	}

	static void Print(const FString& Msg, const FString& InStr)
	{
		Print(FString::Printf(TEXT("%s: %s"), *Msg, *InStr));
	}
}
