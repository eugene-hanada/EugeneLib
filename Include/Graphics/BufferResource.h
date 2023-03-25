#pragma once
#include "GpuResource.h"

namespace Eugene
{
	/// <summary>
	/// バッファ用クラス(頂点、インデックス、定数バッファで使う)
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
		/// <returns> サイズ </returns>
		virtual std::uint64_t GetSize(void) = 0;
	protected:
		BufferResource();
	private:

	};

}