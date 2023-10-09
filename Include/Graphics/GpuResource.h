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

		/// <summary>
		/// マップ可能か？
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual bool CanMap(void) const = 0;

		virtual void* GetResource(void) = 0;
	protected:
		GpuResource();
	};

}
