#include "Cores/CameraUtils.h"

namespace CameraUtil
{
	FVector ClampMaxVerticalDistanceToRay(FVector TestPos, FVector RayOrigin, FVector RayDir, float MaxVerticalDistance)
	{
		// Hypotenuse 斜边：射线原点指向测试点的向量
		FVector Hypotenuse = TestPos - RayOrigin;

		// ParallelComponent：平行于射线的分量，完整保留
		float ProjectionParallel = FVector::DotProduct(Hypotenuse, RayDir);
		FVector ParallelComponent = RayDir * ProjectionParallel;

		// VerticalComponent：垂直于射线的分量，限制最大长度
		FVector VerticalComponent = Hypotenuse - ParallelComponent;

		// 超出最大垂直距离，则缩放垂直分量
		if (VerticalComponent.SizeSquared() > MaxVerticalDistance * MaxVerticalDistance)
		{
			VerticalComponent = VerticalComponent.GetSafeNormal() * MaxVerticalDistance;
		}

		// 重组位置：射线原点 + 平行分量 + 钳位后的垂直分量
		return RayOrigin + ParallelComponent + VerticalComponent;
	}
}
