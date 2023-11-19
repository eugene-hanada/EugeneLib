#pragma once
#include "SoundBase.h"

namespace Eugene
{
	/// <summary>
	/// サウンドの出力をコントロールするクラス(音量、パンなどに使用)
	/// </summary>
	class SoundControl :
		public SoundBase
	{
	public:
		virtual void* Get(void) = 0;
	private:
	};

}