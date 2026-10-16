// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Basic/GA_SwitchCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Comps/GeAbilitySystemComponent.h"
#include "Datas/SwitchCharacterPayload.h"
#include "Tags/EventTags.h"

UGA_SwitchCharacter::UGA_SwitchCharacter()
{
	// 换人是服务端权威行为，客户端只负责把输入传上去
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	SwitchCharacterEventTag = EventTags::Ge_Event_SwitchCharacter;

	// TODO: 可以加上冷却cd 1s
}

void UGA_SwitchCharacter::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !GetOwnerASC())
	{
		K2_EndAbility();
		return;
	}

	// 触发这个 GA 的 InputKey（值来自 EAbilityInputID）
	int32 InputKey = INDEX_NONE;
	if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		InputKey = Spec->InputID;
	}

	if (InputKey == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[GA_SwitchCharacter] %s 不是通过 InputID 授权的，拿不到 InputKey，放弃换人"),
			*GetName());
		K2_EndAbility();
		return;
	}

	// InputKey -> 1-based 队伍槽位。
	// EAbilityInputID::AbilityOne = 3，但对玩家来说它就是"按 1 切第 1 个队友"，
	// 所以不能把枚举值直接当槽位用。
	const int32 SlotIndex = InputKey - static_cast<int32>(FirstSlotInputKey) + 1;

	if (SlotIndex < 1)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[GA_SwitchCharacter] InputKey=%d 换算出的槽位是 %d（< 1），FirstSlotInputKey=%d，放弃换人"),
			InputKey, SlotIndex, static_cast<int32>(FirstSlotInputKey));
		K2_EndAbility();
		return;
	}

	// 打包载荷
	USwitchCharacterPayload* Payload = NewObject<USwitchCharacterPayload>(Avatar);
	Payload->SlotIndex = SlotIndex;
	Payload->SourceAbility = this;

	FGameplayEventData EventData;
	EventData.EventTag = SwitchCharacterEventTag;
	EventData.Instigator = Avatar;
	EventData.Target = Avatar;
	EventData.OptionalObject = Payload;
	// 顺手也塞一份 EventMagnitude，方便只想读浮点数的接收方
	EventData.EventMagnitude = static_cast<float>(SlotIndex);

	// 发给当前角色，由角色处理"销毁自己 + 生成新角色"
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Avatar, SwitchCharacterEventTag, EventData
	);

	K2_EndAbility();
}
