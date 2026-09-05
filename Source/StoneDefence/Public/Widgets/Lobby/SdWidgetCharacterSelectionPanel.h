// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Cores/SdCommonActivatableWidgetBase.h"
#include "SdWidgetCharacterSelectionPanel.generated.h"

class UCommonVisibilitySwitcher;
class UCommonActivatableWidgetSwitcher;
class UCommonTextBlock;
class USdWidgetFaceSculpting;
class ASdActorPreview;
class UCommonListView;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdWidgetCharacterSelectionPanel : public USdCommonActivatableWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	virtual void OnSetParentWidget() override;
	
private:
	/** Binding Widgets */
	UPROPERTY(meta = (BindWidget))
	// UCommonActivatableWidgetSwitcher* Switcher;
	UCommonVisibilitySwitcher* Switcher;
	
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* PanelTitle;
	
	UPROPERTY(meta = (BindWidget))
	UCommonListView* SelectionListView;
	
	UPROPERTY(meta = (BindWidget))
	USdWidgetFaceSculpting* FaceSculptingWidget;
	/********************/

	void InitSelectionListView();

	void CharacterSelected(UObject* SelectedUObject);

	void SpawnCharacterPreview();

	UPROPERTY()
	ASdActorPreview* ActorLobbyPreview;

	UPROPERTY(EditDefaultsOnly, Category="Character Preview")
	TSubclassOf<ASdActorPreview> ActorLobbyPreviewClass;

public:
	void BackToCharacterSelectionPanel();

	void UpdateCharacterAppearances();
	void SelectRecentCharacter();

	void SetPreviewActorIsModifying(bool bIsModifying);

private:
	bool bInitCharacterDefinitions = false;
	bool bPendingUpdateAppearances = false;
};
