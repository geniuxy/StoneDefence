#pragma once

#include "CoreMinimal.h"

namespace CameraUtil
{
	/**
	 * @brief 将输入位置钳位到射线的最大垂直距离，沿射线方向保持不变
	 * @param TestPos 待矫正的相机位置
	 * @param RayOrigin 射线原点
	 * @param RayDir 射线方向，必须是单位向量
	 * @param MaxVerticalDistance 允许最大垂直距离(UU)
	 * @return 矫正后的位置
	 */
	FVector ClampMaxVerticalDistanceToRay(
		FVector TestPos, FVector RayOrigin, FVector RayDir, float MaxVerticalDistance
	);
}
