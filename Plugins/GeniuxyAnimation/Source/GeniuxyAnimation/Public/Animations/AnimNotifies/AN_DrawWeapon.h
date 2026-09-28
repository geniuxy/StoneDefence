// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_DrawWeapon.generated.h"


UCLASS(Blueprintable)
class GENIUXYANIMATION_API USocketNameWrapper : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FName SocketName;
};

/**
 * 
 */
UCLASS()
class GENIUXYANIMATION_API UAN_DrawWeapon : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

private:
	UPROPERTY(EditAnywhere, Category="AnimNotify Details")
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere, Category="AnimNotify Details")
	bool bDrawWeapon = true;
};
