// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Login/SdWidgetRegisterInfo.h"

#include "CommonTextBlock.h"
#include "Widgets/Components/Button/SdCommonButtonImage.h"
#include "Widgets/Login/SdWidgetLoginMain.h"

#define LOCTEXT_NAMESPACE "USdWidgetRegisterInfo"

void USdWidgetRegisterInfo::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Submit->OnReleased().AddUObject(this, &ThisClass::Submit);
	Button_Cancel->OnReleased().AddUObject(this, &ThisClass::Cancel);

	ResetColor();

	SetVisibility(ESlateVisibility::Collapsed);
}

void USdWidgetRegisterInfo::NativeDestruct()
{
	Super::NativeDestruct();
}

void USdWidgetRegisterInfo::Submit()
{
	if (USdWidgetLoginMain* InLoginMain = GetParentWidget<USdWidgetLoginMain>())
	{
		ResetColor();

		FString NiceNameString = EditableText_NiceName->GetText().ToString();
		FString EmailString = EditableText_Email->GetText().ToString();
		FString PlayerAccountString = EditableText_Account->GetText().ToString();
		FString PlayerPasswordString = EditableText_Password->GetText().ToString();
		FString ConfirmPasswordString = EditableText_ConfirmPassword->GetText().ToString();
		FString AuthorURLString = EditableText_AuthorURL->GetText().ToString();
  
		if (IsSpecialCharacters(NiceNameString))
		{
			ShowNiceNameWarnings();

			// WarningPrint(LOCTEXT("SpecialCharacters_NiceName", "This name cannot contain special characters."));

			return;
		}
		else if (IsSpecialCharacters(PlayerAccountString))
		{
			ShowAccountWarnings();

			// WarningPrint(LOCTEXT("SpecialCharacters_Account", "Account with special characters."));

			return;
		}
		else if (IsSpecialCharacters(PlayerPasswordString))
		{
			ShowPasswordWarnings();

			// WarningPrint(LOCTEXT("SpecialCharacters_Password", "The password contains special characters."));

			return;
		}

		if (NiceNameString.Len() < 3)
		{
			ShowNiceNameWarnings();

			// WarningPrint(LOCTEXT("Register_NiceName", "The name has at least 3 characters."));

			return;
		}
		else if (PlayerAccountString.Len() < 8 || PlayerAccountString.Len() > 20)
		{
			ShowAccountWarnings();

			// WarningPrint(LOCTEXT("Register_Account", "The account has at least 8 characters."));

			return;
		}
		else if (!(EmailString.Contains(TEXT(".")) && EmailString.Contains(TEXT("@")) && EmailString.Len() >= 8))
		{
			ShowEmailWarnings();

			// WarningPrint(LOCTEXT("Register_Email", "The mailbox format is incorrect or mail len >= 8"));

			return;
		}
		else if (PlayerPasswordString.Len() < 6|| PlayerPasswordString.Len() > 18)
		{
			ShowPasswordWarnings();
			
			// WarningPrint(LOCTEXT("Register_Password", "The password has at least 6 characters."));

			return;
		}
		else if (PlayerPasswordString != ConfirmPasswordString)
		{
			ShowConfirmPasswordWarnings();
			
			// WarningPrint(LOCTEXT("Register_ConfirmPassword", "The password is inconsistent."));

			return;
		}
		else
		{
			FString RegisterInfo = 
				FString::Printf(TEXT(
					"NiceName=%s&Email=%s&Account=%s&Password=%s&AuthorURL=%s"),
				*NiceNameString,
				*EmailString,
				*PlayerAccountString,
				*PlayerPasswordString,
				*AuthorURLString);

			InLoginMain->Register(RegisterInfo);

			RegisterOut();
		}

		ShowFailedRegisterWarnings();
	}
}

void USdWidgetRegisterInfo::Cancel()
{
	RegisterOut();
}

void USdWidgetRegisterInfo::ResetText()
{
	EditableText_NiceName->SetText(FText::GetEmpty());
	EditableText_Account->SetText(FText::GetEmpty());
	EditableText_Password->SetText(FText::GetEmpty());
	EditableText_ConfirmPassword->SetText(FText::GetEmpty());
	EditableText_AuthorURL->SetText(FText::GetEmpty());
	EditableText_Email->SetText(FText::GetEmpty());
}

void USdWidgetRegisterInfo::ResetColor()
{
	NiceNameText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	PlayerAccountText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	PlayerPasswordText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ConfirmPasswordText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	EmailText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
}

bool USdWidgetRegisterInfo::IsSpecialCharacters(const FString& InString)
{
	const TCHAR *Str = InString.GetCharArray().GetData();

	// 是否有除数字和大/小写字母以外的字符
	if (Str)
	{
		for (; *Str; Str++)
		{
			if ((*Str >= 0x20 && *Str <= 0x2F) ||
				(*Str >= 0x3A && *Str <= 0x3F) ||
				(*Str >= 0x5B && *Str <= 0x60) ||
				(*Str >= 0x7B && *Str <= 0xFF))
			{
				return true;
			}
		}
	}

	return false;
}

void USdWidgetRegisterInfo::RegisterIn()
{
}

void USdWidgetRegisterInfo::RegisterOut()
{
}

void USdWidgetRegisterInfo::ShowFailedRegisterWarnings()
{
}

void USdWidgetRegisterInfo::ShowNiceNameWarnings()
{
	NiceNameText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
}

void USdWidgetRegisterInfo::ShowAccountWarnings()
{
	PlayerAccountText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
}

void USdWidgetRegisterInfo::ShowEmailWarnings()
{
	EmailText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
}

void USdWidgetRegisterInfo::ShowPasswordWarnings()
{
	PlayerPasswordText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
}

void USdWidgetRegisterInfo::ShowConfirmPasswordWarnings()
{
	ConfirmPasswordText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
}

void USdWidgetRegisterInfo::ShowDuplicateWarnings()
{
	ShowEmailWarnings();
	ShowAccountWarnings();
}

#undef LOCTEXT_NAMESPACE
