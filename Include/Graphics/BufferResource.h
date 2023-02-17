#pragma once
#include "GpuResource.h"

namespace Eugene
{
	/// <summary>
	/// バッファ用リソース
	/// </summary>
	class BufferResource :
		public GpuResource
	{
	public:
		virtual ~BufferResource();

		/// <summary>
		/// サイズを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> サイズ(バイト数) </returns>
		virtual std::uint64_t GetSize(void) = 0;
	protected:
		BufferResource();
	private:

	};

}