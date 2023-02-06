#pragma once
#include "../DirectX12/Dx12GpuResource.h"
#include "../../../Include/Graphics/GraphicsCommon.h"
#include "../../../Include/Math/Vector2.h"

struct ID3D12Device;

namespace Eugene
{
	class Image;
	class Dx12UploadableResource :
		public Dx12GpuResource
	{
	public:
		Dx12UploadableResource(ID3D12Device* device,size_t size);
		Dx12UploadableResource(ID3D12Device* device, Image& texture);
		Dx12UploadableResource(ID3D12Device* device, const Vector2& size, Format format);
	private:
		void* Map(void) final;
		void UnMap(void) final;
		bool CanMap(void) const final;
	};
}

