// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Types/GeEnums.h"
#include "GeCommonButtonBase.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS(Blueprintable)
class GENIUXYCOMMON_API UGeCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(FText InText, ESdTextJustify InJustification = ESdTextJustify::Left);

	UFUNCTION(BlueprintCallable)
	FText GetButtonDisplayText() const;
	
protected:
	//~Begin UUserWidget UFunction
	virtual void NativePreConstruct() override;
	//~End UUserWidget UFunction

	//~Begin UCommonButtonBase UFunction
	virtual void NativeOnCurrentTextStyleChanged() override;
	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;
	//~End UCommonButtonBase UFunction
	
	//***** Bound Widgets *****//
	UPROPERTY(meta=(BindWidgetOptional))
	UCommonTextBlock* CommonButtonTextBlock;
	//***** Bound Widgets *****//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	FText ButtonDisplayText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	ESdTextJustify ButtonDisplayTextJustification = ESdTextJustify::Center;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	FText ButtonDescriptionText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	bool bUseUpperCaseForButtonText = false;
};
