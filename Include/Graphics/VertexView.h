#pragma once

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
	protected:
		VertexView();
	};
}
