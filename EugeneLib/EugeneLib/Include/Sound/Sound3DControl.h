#pragma once
#include "SoundBase.h"

namespace EugeneLib
{
	class Sound3DControl :
		public SoundBase
	{
	public:
	 virtual void Update3DSound(const Vector3& emitterPos);
	private:

	};
}
