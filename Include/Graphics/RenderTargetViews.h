#pragma once
#include <cstdint>
#include "GraphicsCommon.h"

namespace Eugene
{
	class Graphics;
	class ImageResource;
	class RenderTargetViews
	{
	public:
		virtual ~RenderTargetViews();
		virtual void Create(ImageResource& resource,std::uint64_t idx, const Format& format) = 0;
		virtual void* GetViews(void) const = 0;
		const std::uint64_t GetSize(void);
	protected:
		RenderTargetViews(std::uint64_t size);
		std::uint64_t size_;
	};

}