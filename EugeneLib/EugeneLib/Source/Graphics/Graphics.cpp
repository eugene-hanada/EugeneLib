#include "../../Include/Graphics/Graphics.h"
#include "../../Source/Graphics/DirectX12/Dx12Graphics.h"

EugeneLib::Graphics::~Graphics()
{
}

void* EugeneLib::Graphics::GetDevice(void) const
{
	return nullptr;
}

void* EugeneLib::Graphics::GetSwapChain(void) const
{
	return nullptr;
}

EugeneLib::Graphics::Graphics()
{
}

EugeneLib::Graphics* EugeneLib::CreateGraphics(System& system, GpuEngine*& outGpuEngine)
{
	return new Dx12Graphics{system, outGpuEngine };
}
