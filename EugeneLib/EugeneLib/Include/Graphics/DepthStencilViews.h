#pragma once
#include "GraphicsCommon.h"

namespace Eugene
{
	class Graphics;
	class GpuResource;

	class DepthStencilViews
	{
	public:
		virtual ~DepthStencilViews();
		virtual void Create(GpuResource& resource, size_t idx) = 0;
		virtual void* GetViews(void) const = 0;
	protected:
		DepthStencilViews(size_t size);
		size_t size_;
	};
}