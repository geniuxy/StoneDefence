// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/Button/GeCommonButtonBase.h"

#include "CommonTextBlock.h"

void UGeCommonButtonBase::SetButtonText(FText InText, ESdTextJustify InJustification)
{
	if (CommonButtonTextBlock && !InText.IsEmpty())
	{
		CommonButtonTextBlock->SetText(bUseUpperCaseForButtonText ? InText.ToUpper() : InText);
		switch (InJustification)
		{
		case ESdTextJustify::Left:
			CommonButtonTextBlock->SetJustification(ETextJustify::Left);
			break;
		case ESdTextJustify::Center:
			CommonButtonTextBlock->SetJustification(ETextJustify::Center);
			break;
		case ESdTextJustify::Right:
			CommonButtonTextBlock->SetJustification(ETextJustify::Right);
			break;
		}
	}
}

FText UGeCommonButtonBase::GetButtonDisplayText() const
{
	if (CommonButtonTextBlock)
	{
		return CommonButtonTextBlock->GetText();
	}
	return FText();
}

void UGeCommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!ButtonDisplayText.IsEmpty())
	{
		SetButtonText(ButtonDisplayText, ButtonDisplayTextJustification);
	}
}

void UGeCommonButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	if (CommonButtonTextBlock && GetCurrentTextStyleClass())
	{
		CommonButtonTextBlock->SetStyle(GetCurrentTextStyleClass());
	}
}

void UGeCommonButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();

	if (!ButtonDescriptionText.IsEmpty())
	{
		// UDkUISubsystem::Get(this)->OnButtonDescriptionTextUpdated.Broadcast(this, ButtonDescriptionText);
	}
}

void UGeCommonButtonBase::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();

	// UDkUISubsystem::Get(this)->OnButtonDescriptionTextUpdated.Broadcast(this, FText::GetEmpty());
}
