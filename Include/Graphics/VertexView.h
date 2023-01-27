#pragma once

namespace Eugene
{
	class VertexView
	{
	public:
		virtual ~VertexView();
		virtual void* GetView(void) const = 0;
	protected:
		VertexView();
	};
}
