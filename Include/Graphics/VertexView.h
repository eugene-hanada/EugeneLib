#pragma once
#include <cstdint>

namespace Eugene
{
	/// <summary>
	/// 頂点ビュー
	/// </summary>
	class VertexView
	{
	public:
		virtual ~VertexView();
		virtual void* GetView(void) const = 0;
		const std::uint32_t GetVertexNum(void) const;
	protected:
		VertexView(std::uint32_t vertexNum);
		std::uint32_t vertexNum_;
	};
}
