#include "Comps/GeAnimationComponent.h"

#include "Data/CharacterAnimationSet.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Character.h"

UGeAnimationComponent::UGeAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGeAnimationComponent::SetAnimationSet(UCharacterAnimationSet* InAnimationSet)
{
	AnimationSet = InAnimationSet;
}

bool UGeAnimationComponent::PlayAction(FGameplayTag ActionTag)
{
	if (!AnimationSet)
	{
		return false;
	}

	const FCharacterMontageEntry* FoundEntry = AnimationSet->FindEntry(ActionTag);

	if (!FoundEntry || FoundEntry->Montage.IsNull())
	{
		return false;
	}

	// Montage 已经加载
	if (FoundEntry->Montage.IsValid())
	{
		return PlayLoadedMontage(*FoundEntry);
	}

	// 拷贝一份，避免异步回调中持有临时指针
	const FCharacterMontageEntry EntryCopy = *FoundEntry;
	const FSoftObjectPath MontagePath = EntryCopy.Montage.ToSoftObjectPath();

	TWeakObjectPtr<UGeAnimationComponent> WeakThis(this);

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		MontagePath,
		FStreamableDelegate::CreateLambda(
			[WeakThis, EntryCopy]()
			{
				if (!WeakThis.IsValid())
				{
					return;
				}

				WeakThis->PlayLoadedMontage(EntryCopy);
			}
		)
	);

	return true;
}

bool UGeAnimationComponent::PlayLoadedMontage(const FCharacterMontageEntry& Entry)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return false;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		return false;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	UAnimMontage* Montage = Entry.Montage.Get();

	if (!AnimInstance || !Montage)
	{
		return false;
	}

	const float Duration = AnimInstance->Montage_Play(Montage, Entry.PlayRate);

	if (Duration <= 0.0f)
	{
		return false;
	}

	if (Entry.StartSection != NAME_None)
	{
		AnimInstance->Montage_JumpToSection(
			Entry.StartSection,
			Montage
		);
	}

	return true;
}

