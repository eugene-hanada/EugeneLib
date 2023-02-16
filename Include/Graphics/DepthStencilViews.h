#pragma once
#include "GraphicsCommon.h"

namespace Eugene
{
	class Graphics;
	class ImageResource;

	class DepthStencilViews
	{
	public:
		virtual ~DepthStencilViews();
		virtual void Create(ImageResource& resource, std::uint64_t idx) = 0;
		virtual void* GetViews(void) const = 0;
	protected:
		DepthStencilViews(std::uint64_t size);
		std::uint64_t size_;
	};
}