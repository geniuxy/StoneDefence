// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeniuxyGASType.h"
#include "Abilities/Core/GeGameplayAbilityBase.h"
#include "Tags/MontageTags.h"
#include "GA_Dodge.generated.h"

class UAnimMontage;
/**
 * 四向闪避
 *
 * 1) 方向从哪来
 *    相对**角色自身**算：用角色的前 / 右（飞行时再加上下）轴去投影当前输入方向，取主轴。
 *    所以「人物朝左时往左按」= 角色的前方 → 播 U；侧移才是 L / R。
 *    输入方向取自 CharacterMovement 的加速度（服务端上同样有，由客户端上行输入还原），
 *    两端算出来的方向天然一致，不用再加一个 RPC 传方向。
 *    如果更想要「相机相对」（W→U、A→L），把 bUseCameraRelativeDirection 勾上即可。
 *
 * 2) 位移从哪来
 *    默认由能力自己推：ApplyRootMotionConstantForce 在 DodgeDuration 内叠一段闪避方向的冲量，
 *    蒙太奇播放时把动画 RootMotion 的位移缩放成 0，所以两者不会叠加，四向距离也可控。
 *    想改成「动画自带 RootMotion 负责位移」，把 bUseMontageRootMotion 勾上即可。
 *
 * 3) 朝向
 *    默认**不改角色 RotationRate**，只在闪避位移那段时间暂停 CMC 的 bOrientRotationToMovement，
 *    避免侧闪被甩成正前方翻滚；位移结束（或把 bLockRotationDuringDodge 关掉）就交还朝向。
 *    方向相对角色自身算，动画自带的 RootMotion 旋转照常生效。
 */
UCLASS()
class GENIUXYGAS_API UGA_Dodge : public UGeGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Dodge();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	virtual bool PreLoadMontage() override;

	/**
	 * 条件式的前置判断：
	 * - 地面（没有 Ge.Stats.Fly）：可以闪；
	 * - 飞行中：必须还有 Ge.Stats.Fly.Fast（加速飞行），否则直接没反应。
	 */
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

private:
	/**
	 * 算方向、判断地面还是飞行、定出世界位移方向。
	 * 返回 false 说明这个角色根本没法闪避。
	 */
	bool BuildDodgeContext();

	/** 当前方向对应的蒙太奇 Tag */
	FGameplayTag GetDodgeMontageTag() const;

	/** 把角色朝到视角方向 */
	void FaceViewDirection();
	/** 闪避期间锁 / 解锁朝向（实质是关掉 CMC 的 bOrientRotationToMovement） */
	void SetRotationLocked(bool bLocked);

	void PlayDodgeMontage();

	void StartFallbackDash();
	float GetEffectiveDodgeDistance();

	UFUNCTION()
	void HandleMontageFinished();

	/** 蒙太奇被打断：别再等位移了，直接收尾 */
	UFUNCTION()
	void HandleMontageCancelled();

	UFUNCTION()
	void HandleDashFinished();

	/** 动画和位移都做完了才结束能力 */
	void TryFinishAbility();

	/**********************************************************************/
	/*                            Montage Config                          */
	/**********************************************************************/
	/** 地面四向闪避蒙太奇：方向 -> MontageMap 里的 Tag */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Montage", meta = (Categories = "Ge.Montage"))
	TMap<EDodgeDirection, FGameplayTag> GroundDodgeMontageTags;

	/** 飞行四向闪避蒙太奇：方向 -> Tag（Forward/Backward 对应 U/D）；整张表留空则回退到地面那套 */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Montage", meta = (Categories = "Ge.Montage"))
	TMap<EDodgeDirection, FGameplayTag> FlyDodgeMontageTags;

	/** 位移距离（bUseMontageRootMotion 打开时才会被动画的位移取代）；填 0 表示只播动画不位移 */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Movement", meta = (ClampMin = "0"))
	float DodgeDistance = 450.f;

	/** 兜底位移的时长 */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Movement", meta = (ClampMin = "0.01"))
	float DodgeDuration = 0.35f;

	/**
	 * 位移由谁负责。
	 * - false（默认）：蒙太奇只负责表现（播放时把动画 RootMotion 的位移缩放成 0），
	 *   位移一律按 DodgeDistance / DodgeDuration 推。四向距离可控，也不受动画有没有 RootMotion 影响。
	 * - true：交给蒙太奇自带的 RootMotion 位移（动画里得有位移轨，否则角色不会动）。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Movement")
	bool bUseMontageRootMotion = false;

	/** 加速飞行（Ge.Stats.Fly.Fast）状态下，位移距离的倍率 */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Movement", meta = (ClampMin = "0"))
	float SprintDodgeDistanceScale = 1.5f;

	/** 一点方向输入都没有时往哪闪（默认向后 / 向下） */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Movement")
	EDodgeDirection DefaultDodgeDirection = EDodgeDirection::Backward;

	/**********************************************************************/
	/*                            Direction Space                          */
	/**********************************************************************/
	/**
	 * false（默认）：方向相对**角色自身**算。角色朝左时往左按 = 角色前方 → 播 U。
	 * true：方向相对**相机**算（W→U、A→L、S→D、D→R），需要配合下面的朝向选项一起用。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Direction")
	bool bUseCameraRelativeDirection = false;

	/** 闪避瞬间把角色朝向对齐到相机方向（相机相对方案才需要，默认关） */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Direction")
	bool bFaceViewDirectionOnDodge = false;

	/**
	 * 闪避位移期间暂停 CMC 的 bOrientRotationToMovement（默认开）。
	 *
	 * 不改角色的 RotationRate：只是在位移那 0.3~0.5s 里不让 CMC 把身体甩向位移方向，
	 * 这样侧闪（L/R）不会被「甩」成正前方翻滚；动画自带的 RootMotion 旋转照常生效。
	 * 位移结束就交还朝向。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Direction")
	bool bLockRotationDuringDodge = true;

	/**********************************************************************/
	/*                              Runtime                               */
	/**********************************************************************/
private:
	UPROPERTY()
	UAnimMontage* DodgeMontage = nullptr;

	EDodgeDirection DodgeDirection = EDodgeDirection::Forward;

	/** 世界空间位移方向（已经收敛到四向主轴，不会斜着走） */
	FVector DodgeWorldDirection = FVector::ForwardVector;

	bool bIsFlying = false;

	bool bRotationLocked = false;

	/** 兜底位移开始时的位置，只用来打日志确认 RootMotion 到底有没有推起来 */
	FVector DashStartLocation = FVector::ZeroVector;

	/** 激活那一刻 CMC 给的方向（只用于日志排查，不参与逻辑） */
	float DebugDodgeAcceleration = 0.f;

	bool bMontageFinished = true;
	bool bDashFinished = true;
};
