// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/Button/GeCommonButtonImage.h"

#include "CommonLazyImage.h"
#include "Kismet/KismetSystemLibrary.h"

void UGeCommonButtonImage::SetButtonDisplayImage(const FSlateBrush& InBrush)
{
	if (CommonLazyImage_ButtonImage)
	{
		CommonLazyImage_ButtonImage->SetBrush(InBrush);
	}
}

void UGeCommonButtonImage::SetButtonDisplayImage(UTexture2D* InTexture)
{
	if (CommonLazyImage_ButtonImage)
	{
		CommonLazyImage_ButtonImage->SetBrushFromTexture(InTexture);
	}
}

void UGeCommonButtonImage::SetButtonDisplayImage(const TSoftObjectPtr<UTexture2D>& InTexture)
{
	if (CommonLazyImage_ButtonImage)
	{
		CommonLazyImage_ButtonImage->SetBrushFromLazyTexture(InTexture);
	}
}

void UGeCommonButtonImage::ToggleHighlightState(bool bShouldHighlight)
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

void UGeCommonButtonImage::NativePreConstruct()
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

void UGeCommonButtonImage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CommonLazyImage_ButtonImage)
	{
		CommonLazyImage_ButtonImage->SetColorAndOpacity(DefaultButtonImageColor);
	}
}

void UGeCommonButtonImage::NativeOnHovered()
{
	Super::NativeOnHovered();

	if (bShouldToggleStateWhenHovered)
	{
		ToggleHighlightState(true);

		if (HoveredSoftButtonImage.ToSoftObjectPath().IsValid())
		{
			SetButtonDisplayImage(HoveredSoftButtonImage);
		}
	}
}

void UGeCommonButtonImage::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();

	if (bShouldToggleStateWhenHovered)
	{
		ToggleHighlightState(false);

		if (HoveredSoftButtonImage.ToSoftObjectPath().IsValid() && SoftButtonImage.ToSoftObjectPath().IsValid())
		{
			SetButtonDisplayImage(SoftButtonImage);
		}
	}
}
