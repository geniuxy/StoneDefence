// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_AttributeModifier.generated.h"

/**
 * 公共基类：「给目标身上的某个属性做 Add / Multiply / Divide / Override」的 GE。
 *
 * 为什么需要这个基类（和 UGE_GrantTags 一个思路）：
 * 蓝图 GE 里「改属性」就是往 Modifiers 数组里塞一条 FGameplayModifierInfo，
 * 而这个数组只是 UGameplayEffect 上的一个 UPROPERTY —— C++ 里完全能写。
 * 并且它没有 GE_GrantTags 那种缓存坑：运行时 FGameplayEffectSpec::Initialize 是
 * 直接读 UGameplayEffect::Modifiers 的，不需要任何「刷新缓存」的补丁动作。
 *
 * 用 C++ GE 而不是蓝图 GE 的好处：
 *  - 属性引用编译期检查：UGeAttributeSetBase::GetMoveSpeedAttribute()；
 *    属性改名、属性集换了会导致编译不过，而蓝图里 Attribute 是从字符串列表里选的；
 *  - 数值 / 运算方式跟能力代码在一起，git diff 看得见；
 *  - 能力构造函数里直接 IncreaseMoveSpeedEffect = UGE_Swim_Fast_Speed::StaticClass()，
 *    不用再让策划在蓝图里指定一个 TSubclassOf 资产。
 *
 * 代价（决定用之前先确认能接受）：
 *  - 原生类不是资产，蓝图 Details 面板里改不了数值。想调参：
 *    a) 做它的蓝图子类（Modifiers 是 EditDefaultsOnly，蓝图子类可以覆盖）；
 *    b) 或者直接改 C++ 重编译。
 *  - 数值走 FScalableFloat：可以给常数，也可以给 CurveTable 行做等级成长。
 *
 * 用法（C++ 子类，构造函数里声明要改什么）：
 *
 *   UGE_Swim_Fast_Speed::UGE_Swim_Fast_Speed()
 *   {
 *       // 游速 × 1.5
 *       AddMultiplicativeModifier(UGeAttributeSetBase::GetMoveSpeedAttribute(), 1.5f);
 *   }
 *
 * 用法（蓝图子类，直接建 GE 资产）：
 *   继承本类后自己在 Modifiers 数组里加条目即可。基类构造函数只在 CDO 上跑一次，
 *   资产里配的 Modifiers 是序列化覆盖，不会被基类重建掉。
 *
 * 时长固定 Infinite：这类「加移速 / 加攻」都是「挂上就一直生效，谁挂上谁摘掉」，
 * 由挂的人拿着 Apply 返回的 FActiveGameplayEffectHandle 调 RemoveActiveGameplayEffect
 * （跟 GE_GrantTags 的约定一致）。要 HasDuration / Instant 的子类自己改 DurationPolicy。
 *
 * 注意 Multiplicitive 的语义：Magnitude 是**倍率**，1.5 = +50%，不是 50%。多个乘算 GE
 * 之间也是相乘的（1.5 × 1.5 = 2.25），加算 GE 则是相加 —— 配道具 / Buff 时别无脑乘。
 */
UCLASS(Abstract)
class GENIUXYGAS_API UGE_AttributeModifier : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_AttributeModifier();

protected:
	/**
	 * 追加一条常数修饰器。
	 *
	 * @param Attribute 目标属性，建议用 UGeAttributeSetBase::GetXxxAttribute()
	 * @param ModOp     运算方式：EGameplayModOp::Additive / Multiplicitive / Division / Override
	 * @param Magnitude 数值；Multiplicitive 时是倍率
	 * @return 新修饰器的指针（可以接着设 SourceTags / TargetTags 等），失败返回 nullptr。
	 *         ★ 别跨 AddModifier 保存这个指针：Modifiers 扩容后旧指针会失效。
	 */
	FGameplayModifierInfo* AddModifier(
		const FGameplayAttribute& Attribute,
		EGameplayModOp::Type ModOp,
		float Magnitude
	);

	/** 同上，数值走 FScalableFloat（CurveTable 行），按 GE 等级取值，做等级成长用 */
	FGameplayModifierInfo* AddScalableModifier(
		const FGameplayAttribute& Attribute,
		EGameplayModOp::Type ModOp,
		const FScalableFloat& Magnitude
	);

	/** 加算：属性 = 基础值 + Magnitude */
	FGameplayModifierInfo* AddAdditiveModifier(const FGameplayAttribute& Attribute, float Magnitude);

	/** 乘算：属性 = 基础值 × Magnitude（Magnitude 是倍率，1.5 = +50%） */
	FGameplayModifierInfo* AddMultiplicativeModifier(const FGameplayAttribute& Attribute, float Magnitude);

	/** 除算：属性 = 基础值 ÷ Magnitude */
	FGameplayModifierInfo* AddDivisionModifier(const FGameplayAttribute& Attribute, float Magnitude);

	/** 覆盖：直接把属性设成 Magnitude（初始化属性常用，注意会盖掉其它修饰器） */
	FGameplayModifierInfo* AddOverrideModifier(const FGameplayAttribute& Attribute, float Magnitude);

private:
	/**
	 * 参数合法性检查：属性引用无效 / 数值 NaN / 除数 0 都只喊一声并跳过这条，
	 * 避免配错了以后 GE 在运行时静默不生效、查半天。
	 */
	bool CheckModifierArgs(
		const FGameplayAttribute& Attribute,
		EGameplayModOp::Type ModOp,
		const FScalableFloat& Magnitude
	) const;
};
