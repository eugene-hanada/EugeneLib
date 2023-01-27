#pragma once
#include <cstdint>

namespace Eugene
{
	/// <summary>
	/// グラフィックス等で使うリソース
	/// </summary>
	class GpuResource
	{
	public:

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~GpuResource();

		/// <summary>
		/// リソースをマップする
		/// </summary>
		/// <param name=""></param>
		/// <returns> ポインタ </returns>
		virtual void* Map(void);

		/// <summary>
		/// リソースをアンマップする
		/// </summary>
		/// <param name=""></param>
		virtual void UnMap(void);
		virtual void* GetResource(void) const= 0;
	protected:
		GpuResource();
	};

}
