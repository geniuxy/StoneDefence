// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/PlayerStates/SdPlayerStateLobby.h"

#include "Subsystems/GameInstanceSubsytems/SdGISubsystemLobby.h"

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

TOptional<FSdCharacterAppearance> ASdPlayerStateLobby::GetCachedCharacterAppearance(int32 InSlotIndex) const
{
	const FSdCharacterAppearance* CharacterAppearance = CachedCharacterAppearances.FindByPredicate(
		[&](const FSdCharacterAppearance& CA)
		{
			return CA.SlotIndex == InSlotIndex;
		});

	if (CharacterAppearance)
	{
		return *CharacterAppearance;
	}
	return {};
}

TOptional<FSdCharacterAppearance> ASdPlayerStateLobby::GetCurSelectedCharacterAppearance() const
{
	USdGISubsystemLobby* GISub = USdGISubsystemLobby::Get(this);
	if (!GISub) return {};

	int32 CurSelectedSlotIndex = GISub->GetCurSelectedSlotIndex();
	if (CurSelectedSlotIndex == INDEX_NONE) return {};

	return GetCachedCharacterAppearance(CurSelectedSlotIndex);
}
