// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_DrawWeapon.generated.h"

// USocketNameWrapper（本 Notify 发出的 EventData 载荷）
// 已统一收到本插件的 Types/GeAnimationPayloads.h 里，用到它时 include 那个头文件。

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
