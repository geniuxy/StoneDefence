// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/Modifier/Core/GE_AttributeModifier.h"

#include "AbilitySystemLog.h"

UGE_AttributeModifier::UGE_AttributeModifier()
{
	// 这类 GE 都是「能力挂着、结束摘掉」，不靠时间自然结束。
	// 要时长型 / 瞬发型 GE 的子类，在构造函数里把 DurationPolicy 改掉即可。
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	// 其它默认值（堆叠、周期、执行类）保持引擎默认：本类只负责往 Modifiers 里塞条目。
}

FGameplayModifierInfo* UGE_AttributeModifier::AddModifier(
	const FGameplayAttribute& Attribute,
	EGameplayModOp::Type ModOp,
	float Magnitude)
{
	return AddScalableModifier(Attribute, ModOp, FScalableFloat(Magnitude));
}

FGameplayModifierInfo* UGE_AttributeModifier::AddScalableModifier(
	const FGameplayAttribute& Attribute,
	EGameplayModOp::Type ModOp,
	const FScalableFloat& Magnitude)
{
	if (!CheckModifierArgs(Attribute, ModOp, Magnitude))
	{
		return nullptr;
	}

	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = Attribute;
	ModifierInfo.ModifierOp = ModOp;

	// FScalableFloat 没有配 Curve 时就是纯常数（Value × 1）；
	// 配了 CurveTable 行则按 GE 等级取值，跟蓝图 GE 里选 Scalable Float 完全等价。
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);

	// EvaluationChannel 保持默认（Channel0）：和蓝图 GE Details 里不选 Channel 时一致，
	// 多个 GE 之间的加算 / 乘算聚合顺序才和其它资产对得上，别在这里乱改。

	return &Modifiers[Modifiers.Add(MoveTemp(ModifierInfo))];
}

FGameplayModifierInfo* UGE_AttributeModifier::AddAdditiveModifier(
	const FGameplayAttribute& Attribute,
	float Magnitude)
{
	return AddModifier(Attribute, EGameplayModOp::Additive, Magnitude);
}

FGameplayModifierInfo* UGE_AttributeModifier::AddMultiplicativeModifier(
	const FGameplayAttribute& Attribute,
	float Magnitude)
{
	// 注意：这里传的是倍率，不是百分比。想要 +50% 传 1.5f。
	return AddModifier(Attribute, EGameplayModOp::Multiplicitive, Magnitude);
}

FGameplayModifierInfo* UGE_AttributeModifier::AddDivisionModifier(
	const FGameplayAttribute& Attribute,
	float Magnitude)
{
	return AddModifier(Attribute, EGameplayModOp::Division, Magnitude);
}

FGameplayModifierInfo* UGE_AttributeModifier::AddOverrideModifier(
	const FGameplayAttribute& Attribute,
	float Magnitude)
{
	return AddModifier(Attribute, EGameplayModOp::Override, Magnitude);
}

bool UGE_AttributeModifier::CheckModifierArgs(
	const FGameplayAttribute& Attribute,
	EGameplayModOp::Type ModOp,
	const FScalableFloat& Magnitude) const
{
	// 属性引用无效 = 属性集没注册 / 属性改名 / 跨模块属性集没加载。
	// 这时 GE 照样能挂上，但那条修饰器会被引擎忽略，表现是「GE 在、属性没变」—— 提前喊一声。
	if (!Attribute.IsValid())
	{
		UE_LOG(LogGameplayEffects, Error,
			TEXT("[GE_AttributeModifier] %s 收到无效的 Attribute（属性集未注册或属性名对不上），该修饰器不会生效"),
			*GetName());
		return false;
	}

	if (ModOp == EGameplayModOp::Max)
	{
		UE_LOG(LogGameplayEffects, Error,
			TEXT("[GE_AttributeModifier] %s 的 %s 传了非法的 ModOp（EGameplayModOp::Max），该修饰器不会生效"),
			*GetName(), *Attribute.GetName());
		return false;
	}

	if (ModOp == EGameplayModOp::Division && FMath::IsNearlyZero(Magnitude.Value))
	{
		// 蓝图 GE 里除 0 会算出 Inf 把属性直接搞坏，C++ 这边干脆拒绝。
		UE_LOG(LogGameplayEffects, Error,
			TEXT("[GE_AttributeModifier] %s 对 %s 配置了 Division 但数值为 0，该修饰器不会生效"),
			*GetName(), *Attribute.GetName());
		return false;
	}

	if (FMath::IsNaN(Magnitude.Value))
	{
		UE_LOG(LogGameplayEffects, Error,
			TEXT("[GE_AttributeModifier] %s 对 %s 配置的数值是 NaN，该修饰器不会生效"),
			*GetName(), *Attribute.GetName());
		return false;
	}

	// 配了 CurveTable 行但表 / 行找不到，也提前提示（没配 Curve 的纯常数不算错）。
	if (!Magnitude.Curve.RowName.IsNone() && !Magnitude.IsValidWithWarnings(GetPathName()))
	{
		UE_LOG(LogGameplayEffects, Warning,
			TEXT("[GE_AttributeModifier] %s 对 %s 配置的 CurveTable 行无效，运行时数值可能取不到"),
			*GetName(), *Attribute.GetName());
	}

	return true;
}
