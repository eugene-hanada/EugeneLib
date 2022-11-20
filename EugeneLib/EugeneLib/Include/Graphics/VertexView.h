#pragma once

namespace EugeneLib
{
	class GpuResource;
	class VertexView
	{
	public:
		virtual ~VertexView();
		virtual void* GetView(void) const = 0;
	protected:
		VertexView();
	};

	VertexView* CreateVertexView(size_t size, GpuResource& resource);
}
