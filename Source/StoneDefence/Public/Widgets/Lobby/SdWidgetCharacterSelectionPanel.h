// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/SdCommonUserWidgetBase.h"
#include "SdWidgetCharacterSelectionPanel.generated.h"

class USdWidgetFaceSculpting;
class ASdActorLobbyDisplay;
class UCommonListView;
class UCommonVisibilitySwitcher;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetCharacterSelectionPanel : public USdCommonUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	UCommonVisibilitySwitcher* Switcher;
	
	UPROPERTY(meta = (BindWidget))
	UCommonListView* SelectionListView;
	
	UPROPERTY(meta = (BindWidget))
	USdWidgetFaceSculpting* FaceSculptingWidget;
	/********************/

	void InitSelectionListView();

	void CharacterSelected(UObject* SelectedUObject);

	void SpawnCharacterDisplay();

	UPROPERTY()
	ASdActorLobbyDisplay* ActorLobbyDisplay;

	UPROPERTY(EditDefaultsOnly, Category="Character Display")
	TSubclassOf<ASdActorLobbyDisplay> ActorLobbyDisplayClass;

public:
	void BackToCharacterSelectionPanel();

	void UpdateCharacterAppearances();
	void SelectRecentCharacter();

private:
	bool bInitCharacterDefinitions = false;
	bool bPendingUpdateAppearances = false;
};
