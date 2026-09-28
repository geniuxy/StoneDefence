// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AN_DrawWeapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Tags/AMEventTags.h"

void UAN_DrawWeapon::Notify(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp->GetOwner()) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!OwnerASC) return;

	FGameplayEventData EventData;
	USocketNameWrapper* Wrapper = NewObject<USocketNameWrapper>();
	Wrapper->SocketName = WeaponSocketName;
	EventData.OptionalObject = Wrapper;
	if (bDrawWeapon)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			MeshComp->GetOwner(), AMEventTags::Ge_Event_DrawWeapon, EventData
		);
	}
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			MeshComp->GetOwner(), AMEventTags::Ge_Event_SheatheWeapon, EventData
		);
	}
}
