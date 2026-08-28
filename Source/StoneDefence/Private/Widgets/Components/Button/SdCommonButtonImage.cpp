// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/Button/SdCommonButtonImage.h"

#include "CommonLazyImage.h"
#include "Kismet/KismetSystemLibrary.h"

void USdCommonButtonImage::SetButtonDisplayImage(const FSlateBrush& InBrush)
{
	if (CommonLazyImage_ButtonImage)
	{
		CommonLazyImage_ButtonImage->SetBrush(InBrush);
	}
}

void USdCommonButtonImage::SetButtonDisplayImage(UTexture2D* InTexture)
{
	if (CommonLazyImage_ButtonImage)
	{
		CommonLazyImage_ButtonImage->SetBrushFromTexture(InTexture);
	}
}

void USdCommonButtonImage::ToggleHighlightState(bool bShouldHighlight)
{
	if (!CommonLazyImage_ButtonImage) return;
	
	if (bShouldHighlight)
	{
		CommonLazyImage_ButtonImage->SetColorAndOpacity(HighlightButtonImageColor);
	}
	else
	{
		CommonLazyImage_ButtonImage->SetColorAndOpacity(DefaultButtonImageColor);
	}
}

void USdCommonButtonImage::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (CommonLazyImage_ButtonImage)
	{
		CommonLazyImage_ButtonImage->SetBrushFromLazyTexture(SoftButtonImage);
		CommonLazyImage_ButtonImage->SetDesiredSizeOverride(FVector2D(ButtonImageSize));

		if (IsDesignTime() && UKismetSystemLibrary::IsValidSoftObjectReference(PreviewSoftButtonImage))
		{
			CommonLazyImage_ButtonImage->SetBrushFromLazyTexture(PreviewSoftButtonImage);
		}
	}
}

void USdCommonButtonImage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CommonLazyImage_ButtonImage)
	{
		CommonLazyImage_ButtonImage->SetColorAndOpacity(DefaultButtonImageColor);
	}
}

void USdCommonButtonImage::NativeOnHovered()
{
	Super::NativeOnHovered();

	if (bShouldToggleStateWhenHovered)
	{
		ToggleHighlightState(true);
	}
}

void USdCommonButtonImage::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();

	if (bShouldToggleStateWhenHovered)
	{
		ToggleHighlightState(false);
	}
}
