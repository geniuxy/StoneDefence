// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Login/SdWidgetLoginInfo.h"

#include "Components/EditableTextBox.h"
#include "Stream/SimpleIOStream.h"
#include "Widgets/Components/Button/SdCommonButtonImage.h"
#include "Widgets/Login/SdWidgetLoginMenu.h"

void USdWidgetLoginInfo::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Enter->OnReleased().AddUObject(this, &ThisClass::SignIn);
	Button_Register->OnReleased().AddUObject(this, &ThisClass::Register);
}

void USdWidgetLoginInfo::NativeDestruct()
{
	Super::NativeDestruct();
}

void USdWidgetLoginInfo::SignIn()
{
	if (USdWidgetLoginMenu* LoginMenu = GetParentWidget<USdWidgetLoginMenu>())
	{
		FString AccountStr = EditableText_Account->GetText().ToString();
		FString PasswordStr = EditableText_Password->GetText().ToString();
		LoginMenu->SignIn(AccountStr, PasswordStr);
	}
}

void USdWidgetLoginInfo::Register()
{
	if (USdWidgetLoginMenu* LoginMenu = GetParentWidget<USdWidgetLoginMenu>())
	{
		LoginMenu->Register();
	}
}

bool USdWidgetLoginInfo::EncryptionToLocal(const FString& InPaths)
{
	FString AccountStr = EditableText_Account->GetText().ToString();
	FString PasswordStr = EditableText_Password->GetText().ToString();

	// 路径纠正
	FString InCopyPaths = InPaths;
	FPaths::NormalizeFilename(InCopyPaths); // \\->/
	InCopyPaths.RemoveFromEnd(TEXT("/"));

	if (!AccountStr.IsEmpty() && !PasswordStr.IsEmpty() && !InCopyPaths.IsEmpty())
	{
		TArray<uint8> PasswordBuff;
		FSimpleIOStream Stream(PasswordBuff);

		Stream << PasswordStr;

		FString HashKey = FMD5::HashAnsiString(*AccountStr);
		for (int i = 0; i < 32; ++i)
		{
			HashKey = FMD5::HashAnsiString(*HashKey);
		}

		if (!HashKey.IsEmpty())
		{
			// 补全至16位或者32位
			int32 Remain = 0;
			int32 Total = PasswordBuff.Num();
			if (Total % 16)
			{
				float Value = (float)Total / 16.f;
				int32 Ceil = FMath::CeilToInt(Value);
				int32 CeilTotal = Ceil * 16;
				Remain = CeilTotal - Total;
			}
			for (int i = 0; i < Remain; ++i)
			{
				uint8 Tmp = 0;
				Stream << Tmp;
			}

			ANSICHAR* InKey = TCHAR_TO_UTF8(*HashKey);
			FAES::EncryptData(PasswordBuff.GetData(), PasswordBuff.Num(), InKey);

			// 删除原有文件
			IFileManager::Get().DeleteDirectory(*InCopyPaths);

			FFileHelper::SaveArrayToFile(PasswordBuff, *(InCopyPaths / AccountStr + TEXT(".arpg")));

			return true;
		}
	}

	return false;
}

bool USdWidgetLoginInfo::DecryptionFromLocal(const FString& InPaths)
{
	// 路径纠正
	FString InCopyPaths = InPaths;
	FPaths::NormalizeFilename(InCopyPaths); // \\->/
	InCopyPaths.RemoveFromEnd(TEXT("/"));

	TArray<FString> FileNames;
	IFileManager::Get().FindFilesRecursive(FileNames, *InPaths, TEXT("*.*"), true, false);

	for (auto& Tmp : FileNames)
	{
		FString FileNameTitle = FPaths::GetCleanFilename(Tmp);
		if (FileNameTitle.Contains(TEXT(".arpg")))
		{
			TArray<uint8> InBytes;
			FFileHelper::LoadFileToArray(InBytes, *Tmp);

			FileNameTitle.RemoveFromEnd(TEXT(".arpg"));

			if (InBytes.Num() > 0)
			{
				FString HashKey = FMD5::HashAnsiString(*FileNameTitle);
				for (int i = 0; i < 32; ++i)
				{
					HashKey = FMD5::HashAnsiString(*HashKey);
				}

				if (!HashKey.IsEmpty())
				{
					FSimpleIOStream Stream(InBytes);

					ANSICHAR* InKey = TCHAR_TO_UTF8(*HashKey);
					FAES::DecryptData(InBytes.GetData(), InBytes.Num(), InKey);

					FString PasswordStr;
					Stream >> PasswordStr;

					EditableText_Account->SetText(FText::FromString(FileNameTitle));
					EditableText_Password->SetText(FText::FromString(PasswordStr));

					return true;
				}
			}
		}
	}
	
	return false;
}
