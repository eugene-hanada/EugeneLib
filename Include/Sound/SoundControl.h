#pragma once
#include "SoundBase.h"

namespace Eugene
{
	class SoundControl :
		public SoundBase
	{
	public:
		virtual void* Get(void) = 0;
	private:
	};

}