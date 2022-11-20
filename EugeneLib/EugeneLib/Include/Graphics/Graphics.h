#pragma once

namespace EugeneLib
{
	class System;
	class GpuEngine;
	class RenderTargetViews;
	class CommandList;
	class GpuResource;
	class Graphics
	{
	public:
		virtual ~Graphics();

		virtual void* GetDevice(void) const;
		virtual void* GetSwapChain(void) const;
		virtual GpuResource& GetBackBufferResource(void) = 0;
		virtual RenderTargetViews& GetViews(void) = 0;
		virtual size_t GetNowBackBufferIndex(void) = 0;
		virtual void Present(void) = 0;
	protected:
		Graphics();

	};

	Graphics* CreateGraphics(System& system, GpuEngine*& outGpuEngine);
}
