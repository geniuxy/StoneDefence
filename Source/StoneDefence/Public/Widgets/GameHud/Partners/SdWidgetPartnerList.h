// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Widgets/Cores/GeCommonUserWidgetBase.h"
#include "SdWidgetPartnerList.generated.h"

class ASdCharacterHeroBase;
class UCommonListView;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetPartnerList : public UGeCommonUserWidgetBase
{
	GENERATED_BODY()

public:
	void AddCharacter(FGameplayTag InCharacterTag, bool bSelected = true);

protected:
	//~Begin UUserWidget UFunction
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End UUserWidget UFunction

private:
	UPROPERTY(meta=(BindWidget))
	UCommonListView* CharacterList;

	void CharacterSelected(UObject* SelectedUObject);
};
