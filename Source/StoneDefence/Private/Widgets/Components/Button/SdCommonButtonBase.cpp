// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/Button/SdCommonButtonBase.h"

#include "CommonTextBlock.h"

void USdCommonButtonBase::SetButtonText(FText InText, ESdTextJustify InJustification)
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

FText USdCommonButtonBase::GetButtonDisplayText() const
{
	if (CommonButtonTextBlock)
	{
		return CommonButtonTextBlock->GetText();
	}
	return FText();
}

void USdCommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!ButtonDisplayText.IsEmpty())
	{
		SetButtonText(ButtonDisplayText, ButtonDisplayTextJustification);
	}
}

void USdCommonButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	if (CommonButtonTextBlock && GetCurrentTextStyleClass())
	{
		CommonButtonTextBlock->SetStyle(GetCurrentTextStyleClass());
	}
}

void USdCommonButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();

	if (!ButtonDescriptionText.IsEmpty())
	{
		// UDkUISubsystem::Get(this)->OnButtonDescriptionTextUpdated.Broadcast(this, ButtonDescriptionText);
	}
}

void USdCommonButtonBase::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();

	// UDkUISubsystem::Get(this)->OnButtonDescriptionTextUpdated.Broadcast(this, FText::GetEmpty());
}
