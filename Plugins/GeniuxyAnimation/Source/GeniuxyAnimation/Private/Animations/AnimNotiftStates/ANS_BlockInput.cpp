// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotiftStates/ANS_BlockInput.h"

#include "GameFramework/Character.h"

void UANS_BlockInput::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetController())
		{
			InCharacter->GetController()->SetIgnoreMoveInput(true);
		}
	}
}

void UANS_BlockInput::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetController())
		{
			InCharacter->GetController()->SetIgnoreMoveInput(false);
		}
	}
}
