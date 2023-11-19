#pragma once
#include "SoundControl.h"
#include "../ThirdParty/glm/glm/vec3.hpp"

namespace Eugene
{
	/// <summary>
	/// 3Dサウンドをコントロールするクラス
	/// </summary>
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
			const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity
			,const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity) = 0;
	};
}
