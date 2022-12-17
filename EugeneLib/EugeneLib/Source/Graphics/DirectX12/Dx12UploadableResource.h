#pragma once
#include "../DirectX12/Dx12GpuResource.h"
#include "../../../Include/Graphics/GraphicsCommon.h"

struct ID3D12Device;

namespace EugeneLib
{
	class Texture;
	class Dx12UploadableResource :
		public Dx12GpuResource
	{
	public:
		Dx12UploadableResource(size_t size, ID3D12Device* device);
		Dx12UploadableResource(Texture& texture, ID3D12Device* device);
	private:
		void* Map(void) final;
		void UnMap(void) final;
	};
}

