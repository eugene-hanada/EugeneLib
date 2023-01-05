#pragma once
#include "SoundBase.h"

namespace EugeneLib
{
	class SoundControl :
		public SoundBase
	{
	public:
		virtual void* Get(void) = 0;
	private:
	};

}