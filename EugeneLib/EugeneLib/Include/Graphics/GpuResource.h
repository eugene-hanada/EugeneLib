#pragma once
#include <cstdint>

namespace EugeneLib
{
	class Graphics;
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
	GpuResource* CreateDefaultResource(size_t, Graphics& graphics);
	GpuResource* CreateSwapChainResource(std::uint32_t idx, Graphics& graphics);
}
