#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"

/**
 * 屏幕 + 日志一体的调试输出。
 *
 * 键名（KeyName）机制：
 * - 调 `Debug::Print(Msg, KeyName, Duration, Color)` 时，KeyName 就是一个字符串；
 * - 内部有一张**静态表**（进程内 = 当前游戏实例一份）：键名 -> InKey；
 * - 键名第一次出现时顺序分配一个新的 InKey 并存进表里，之后再出现直接复用 ——
 *   所以同一条消息在屏幕上是「原地刷新」，不会越堆越多行。
 * - 自动分配的 InKey 从 9000 开始，和能力里手工用的号段错开。
 *
 * 例：
 *   Debug::Print(TEXT("[GA_Dodge] 激活"), TEXT("Dodge.Activate"), 4.f, FColor::Green);
 *   Debug::Print(TEXT("[GA_Dodge] 激活"), TEXT("Dodge.Activate"));   // 同一个 Key，原地刷新
 *
 * 打包版本：
 *   只在编辑器 / PIE 里输出（`WITH_EDITOR`）。打包出来的版本（Development / Shipping 都算）
 *   整段编译掉 —— 既不上屏也不写日志。想让调试包也保留输出，把 bEnableInPackagedBuild 改 true。
 *   注意：调用点的字符串拼接（FString::Printf）仍然会执行，重的那条自己用 #if WITH_EDITOR 包一下。
 */
namespace Debug
{
	/**********************************************************************/
	/*                             Compile Switch                         */
	/**********************************************************************/

	/** 打包版（非 Editor）是否也输出；默认 false = 打包后完全静默，true = 打包后也打印信息 */
	inline constexpr bool bEnableInPackagedBuild = false;

	/** 当前构建是否输出调试信息：编辑器里永远输出，打包版看上面的开关 */
	inline constexpr bool IsEnabled()
	{
		return WITH_EDITOR || bEnableInPackagedBuild;
	}
	// (constexpr 的核心作用是将计算从运行时“提前”到编译时完成)

	/**********************************************************************/
	/*                          KeyName -> InKey                          */
	/**********************************************************************/
	/** 键名表：一个进程（= 一个游戏实例）一份，游戏线程访问 */
	inline TMap<FString, int32>& GetKeyMap()
	{
		static TMap<FString, int32> KeyMap;
		return KeyMap;
	}

	/** 下一个要分配的 InKey */
	inline int32& GetNextKey()
	{
		static int32 NextKey = 9000;
		return NextKey;
	}

	/** 键名换成 InKey：存在就复用，不存在就分配一个新的并记下来；空键名返回 -1（交给引擎分配） */
	inline int32 ResolveKey(const FString& InKeyName)
	{
		if (InKeyName.IsEmpty())
		{
			return -1;
		}

		TMap<FString, int32>& KeyMap = GetKeyMap();
		if (const int32* FoundKey = KeyMap.Find(InKeyName))
		{
			return *FoundKey;
		}

		const int32 NewKey = GetNextKey()++;
		KeyMap.Add(InKeyName, NewKey);
		return NewKey;
	}

	/** 清空键名表（一般不用；PIE 重开想从头分配时可以用） */
	inline void ResetKeys()
	{
		GetKeyMap().Reset();
		GetNextKey() = 9000;
	}

	/**********************************************************************/
	/*                               Print                                */
	/**********************************************************************/
	inline void Print(
		const FString& Msg,
		const FColor& InColor = FColor::MakeRandomColor(),
		float Duration = 15.f,
		int32 InKey = -1)
	{
		if constexpr (IsEnabled())
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, Msg);

				UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
			}
		}
		else
		{
			// 打包版：整段编译掉，参数显式忽略，避免未使用参数告警
			(void)Msg;
			(void)InKey;
			(void)Duration;
			(void)InColor;
		}
	}

	inline void Print(
		const FString& Msg,
		const FString& InKeyName,
		float Duration = 15.f,
		const FColor& InColor = FColor::MakeRandomColor())
	{
		if constexpr (IsEnabled())
		{
			Print(Msg, InColor, Duration, ResolveKey(InKeyName));
		}
		else
		{
			(void)Msg;
			(void)InKeyName;
			(void)Duration;
			(void)InColor;
		}
	}

	inline void PrintValue(const FString& Msg, const FString& InValue, const FString& InKeyStr = "")
	{
		if constexpr (IsEnabled())
		{
			if (InKeyStr.IsEmpty())
			{
				Print(FString::Printf(TEXT("%s: %s"), *Msg, *InValue));
			}
			else
			{
				Print(FString::Printf(TEXT("%s: %s"), *Msg, *InValue), InKeyStr);
			}
		}
		else
		{
			(void)Msg;
			(void)InValue;
		}
	}

	inline void PrintValue(const FString& Msg, int32 InValue, const FString& InKeyStr = "")
	{
		if constexpr (IsEnabled())
		{
			if (InKeyStr.IsEmpty())
			{
				Print(FString::Printf(TEXT("%s: %d"), *Msg, InValue));
			}
			else
			{
				Print(FString::Printf(TEXT("%s: %d"), *Msg, InValue), InKeyStr);
			}
		}
		else
		{
			(void)Msg;
			(void)InValue;
		}
	}
}
