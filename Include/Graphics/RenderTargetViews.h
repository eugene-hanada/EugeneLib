#pragma once
#include <cstdint>
#include "GraphicsCommon.h"

namespace Eugene
{
	class Graphics;
	class GpuResource;
	class RenderTargetViews
	{
	public:
		virtual ~RenderTargetViews();
		virtual void Create(GpuResource& resource,size_t idx, const Format& format) = 0;
		virtual void* GetViews(void) const = 0;
		const size_t GetSize(void);
	protected:
		RenderTargetViews(size_t size);
		size_t size_;
	};

}