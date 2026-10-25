// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_Fly.h"

#include "Tags/StatsTags.h"

UGA_Fly::UGA_Fly()
{
	ActivationOwnedTags.AddTag(StatsTags::Ge_Stats_Fly);
}
