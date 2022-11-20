#pragma once

namespace EugeneLib
{
	class Graphics;
	class CommandList;
	class GpuEngine
	{
	public:

		virtual ~GpuEngine();
		virtual void Execute(void) = 0;
		virtual void Wait(void) = 0;
		virtual void Push(CommandList& commandList) = 0;
		virtual void* GetQueue(void) const = 0;
	protected:
		GpuEngine();
	};

	GpuEngine* CreateGpuEngine(size_t maxSize, Graphics& graphics);
}