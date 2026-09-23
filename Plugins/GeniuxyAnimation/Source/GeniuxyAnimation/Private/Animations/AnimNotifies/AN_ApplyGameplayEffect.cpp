// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AN_ApplyGameplayEffect.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"


void UAN_ApplyGameplayEffect::Notify(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp->GetOwner()) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!OwnerASC) return;
	if (!OwnerASC->GetOwner()->HasAuthority()) return;

	if (Effect)
	{
		FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(Effect, 1, OwnerASC->MakeEffectContext());
		OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

FString UAN_ApplyGameplayEffect::GetNotifyName_Implementation() const
{
	if (IsValid(Effect))
	{
		return Effect.Get()->GetName();
	}
	
	return "None";
}
