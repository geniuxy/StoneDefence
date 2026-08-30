// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SdCommonButtonBase.h"
#include "SdCommonButtonImage.generated.h"

class UCommonLazyImage;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API USdCommonButtonImage : public USdCommonButtonBase
{
	GENERATED_BODY()

public:
	void SetButtonDisplayImage(const FSlateBrush& InBrush);
	void SetButtonDisplayImage(UTexture2D* InTexture);
	void SetButtonDisplayImage(const TSoftObjectPtr<UTexture2D>& InTexture);

	UFUNCTION(BlueprintCallable)
	void ToggleHighlightState(bool bShouldHighlight);

protected:
	//~Begin UUserWidget UFunction
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	//~End UUserWidget UFunction

	//~Begin UCommonButtonBase UFunction
	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;
	//~End UCommonButtonBase UFunction
	
	//***** Bound Widgets *****//
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess = "true")) // 用于 C++ 与 UMG 蓝图之间的绑定
	UCommonLazyImage* CommonLazyImage_ButtonImage;
	//***** Bound Widgets *****//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	bool bShouldToggleStateWhenHovered = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	TSoftObjectPtr<UTexture2D> SoftButtonImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	TSoftObjectPtr<UTexture2D> PreviewSoftButtonImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	TSoftObjectPtr<UTexture2D> HoveredSoftButtonImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	float ButtonImageSize = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	FLinearColor DefaultButtonImageColor = FLinearColor(0.4f,0.4f,0.4f,1.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	FLinearColor HighlightButtonImageColor = FLinearColor(1.f,1.f, 1.f,1.f);
};
