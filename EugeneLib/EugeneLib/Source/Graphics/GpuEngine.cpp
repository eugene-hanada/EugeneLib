#include "../../Include/Graphics/GpuEngine.h"
#include "../../Source/Graphics/DirectX12/Dx12GpuEngine.h"

EugeneLib::GpuEngine::~GpuEngine()
{
}

EugeneLib::GpuEngine::GpuEngine()
{
}

EugeneLib::GpuEngine* EugeneLib::CreateGpuEngine(size_t maxSize, Graphics& graphics)
{
    return new Dx12GpuEngine{maxSize, graphics};
}
