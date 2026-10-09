// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GeniuxyCommonType.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeniuxyCommonBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UGeniuxyCommonBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	/*
	 * 在服务器上遍历所有的PlayerController，找到符合InImplement的那个
	 */
	template <class T>
	static void ServerCallAllPlayerController(UWorld* InWorld, TFunction<EServerCallType(T*)> InImplement)
	{
		for (FConstPlayerControllerIterator It = InWorld->GetPlayerControllerIterator(); It; ++It)
		{
			if (T* InPlayerController = Cast<T>(It->Get()))
			{
				if (InImplement(InPlayerController) == EServerCallType::SC_PROGRESS_COMPLETE)
				{
					break;
				}
			}
		}
	}

	template <class T>
	static void ServerCallAllPlayer(UWorld* InWorld, TFunction<EServerCallType(T*)> InImplement)
	{
		ServerCallAllPlayerController<APlayerController>(InWorld, [&](const APlayerController* InPlayerController)
		{
			if (T* InPawn = Cast<T>(InPlayerController->GetPawn()))
			{
				return InImplement(InPawn);
			}

			return EServerCallType::SC_INPROGRESS;
		});
	}

	template <typename EnumType>
	static FString GetStringValueOfEnum(EnumType InEnumType)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();

		return StaticEnumOption->GetNameStringByIndex(static_cast<int64>(InEnumType));
	}

	// Tips: 这个方法不能用于打包出来的版本
	template <typename EnumType>
	static FText GetDisplayValueOfEnum(EnumType InEnumType)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();

		return StaticEnumOption->GetDisplayNameTextByIndex(static_cast<int64>(InEnumType));
	}
};
