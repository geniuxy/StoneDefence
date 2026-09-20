// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/CharacterAnimationSet.h"

const FCharacterMontageEntry* UCharacterAnimationSet::FindEntry(FGameplayTag ActionTag) const
{
	FGameplayTag CurrentTag = ActionTag;

	while (CurrentTag.IsValid())
	{
		if (const FCharacterMontageEntry* Entry = Montages.Find(CurrentTag))
		{
			return Entry;
		}

		CurrentTag = CurrentTag.RequestDirectParent();
	}

	return nullptr;
}
