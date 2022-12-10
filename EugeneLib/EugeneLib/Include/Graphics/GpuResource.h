#pragma once
#include <cstdint>
#include "GraphicsCommon.h"

namespace EugeneLib
{
	class Graphics;
	class Texture;
	class GpuResource
	{
	public:
		virtual ~GpuResource();
		virtual void* Map(void);
		virtual void UnMap(void);
		virtual void* GetResource(void) const= 0;
	protected:
		GpuResource();
	};

	GpuResource* CreateUploadableResource(size_t size, Graphics& graphics);
	GpuResource* CreateUploadableResource(Texture& texture, Graphics& graphics);
	GpuResource* CreateDefaultResource(size_t, Graphics& graphics);
	GpuResource* CreateSwapChainResource(std::uint32_t idx, Graphics& graphics);
	GpuResource* CreateTextureResource(const TextureInfo& formatData, Graphics& graphics);
}
