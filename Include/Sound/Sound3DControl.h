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
		virtual void Update3DSound(
			const Vector3& listenerFront, const Vector3& listenerTop, const Vector3& listenerPos, const Vector3& listenerVeclocity
			,const Vector3& emitterFront, const Vector3& emitterTop, const Vector3& emitterPos, const Vector3& emitterVelocity) = 0;
	};
}
