// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Core/GeGameplayAbilityBase.h"

UAbilitySystemComponent* UGeGameplayAbilityBase::GetOwnerASC()
{
	if (!OwnerASC)
	{
		OwnerASC = GetAbilitySystemComponentFromActorInfo();
	}

	return OwnerASC;
}
