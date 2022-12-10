#include "../../Include/Graphics/GpuResource.h"
#include "../../Source/Graphics/DirectX12/Dx12UploadableResource.h"
#include "../../Source/Graphics/DirectX12/Dx12DefaultResource.h"

EugeneLib::GpuResource::~GpuResource()
{
}

void* EugeneLib::GpuResource::Map(void)
{
	return nullptr;
}

void EugeneLib::GpuResource::UnMap(void)
{
}

EugeneLib::GpuResource::GpuResource()
{
}

EugeneLib::GpuResource* EugeneLib::CreateUploadableResource(size_t size, Graphics& graphics)
{
	return new Dx12UploadableResource{size,graphics};
}

EugeneLib::GpuResource* EugeneLib::CreateUploadableResource(Texture& texture, Graphics& graphics)
{
	return new Dx12UploadableResource{ texture, graphics};
}

EugeneLib::GpuResource* EugeneLib::CreateDefaultResource(size_t size, Graphics& graphics)
{
	return new Dx12DefaultResource{size,graphics};
}

EugeneLib::GpuResource* EugeneLib::CreateSwapChainResource(std::uint32_t idx, Graphics& graphics)
{
	return new Dx12DefaultResource{idx,graphics};
}

EugeneLib::GpuResource* EugeneLib::CreateTextureResource(const TextureInfo& formatData, Graphics& graphics)
{
	return new Dx12DefaultResource{formatData, graphics };
}
