#pragma once
#include <cstdint>

namespace Eugene
{
	/// <summary>
	/// インデックス用のビュー
	/// </summary>
	class IndexView
	{
	public:
		virtual ~IndexView();
		virtual void* GetView(void) const = 0;

		/// <summary>
		/// インデックス数を取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> インデックス数 </returns>
		const std::uint32_t GetIndexNum(void) const;
	protected:
		IndexView(std::uint32_t indexNum);
		std::uint32_t indexNum_;
	};
}
