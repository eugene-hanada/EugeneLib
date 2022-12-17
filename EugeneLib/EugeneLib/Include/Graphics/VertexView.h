#pragma once

namespace EugeneLib
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
