#pragma once
#include "SoundControl.h"
#include "../Math/Vector3.h"

namespace Eugene
{
	class Sound3DControl :
		public SoundControl
	{
	public:
		virtual ~Sound3DControl();

		/// <summary>
		/// 3Dサウンドの情報を更新する
		/// </summary>
		/// <param name="listenerFront"> リスナーの正面方向 </param>
		/// <param name="listenerTop"> リスナーの上向き </param>
		/// <param name="listenerPos"> リスナーの座標 </param>
		/// <param name="listenerVeclocity"> リスナーの速度 </param>
		/// <param name="emitterFront"> エミッタの正面方向 </param>
		/// <param name="emitterTop"> エミッタの上方向 </param>
		/// <param name="emitterPos"> エミッタの座標 </param>
		/// <param name="emitterVelocity"> エミッタの速度 </param>
		virtual void Set3DSound(
			const Vector3& listenerFront, const Vector3& listenerTop, const Vector3& listenerPos, const Vector3& listenerVeclocity
			,const Vector3& emitterFront, const Vector3& emitterTop, const Vector3& emitterPos, const Vector3& emitterVelocity) = 0;
	};
}
