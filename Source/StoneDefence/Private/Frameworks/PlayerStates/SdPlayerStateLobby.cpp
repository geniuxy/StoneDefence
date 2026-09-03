// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/PlayerStates/SdPlayerStateLobby.h"

void ASdPlayerStateLobby::SetSelectedCharacterDefinition(const UPA_CharacterDefinition* NewDefinition)
{
}

void ASdPlayerStateLobby::UpdateCharacterAppearances(const FSdCharacterAppearance& InCA)
{
	FSdCharacterAppearance* CurCharacterAppearance = CachedCharacterAppearances.FindByPredicate(
		[&](const FSdCharacterAppearance& CA)
		{
			return CA.SlotIndex == InCA.SlotIndex;
		}
	);

	if (CurCharacterAppearance)
	{
		*CurCharacterAppearance = InCA;
	}
	else
	{
		CachedCharacterAppearances.Add(InCA);
	}
}
