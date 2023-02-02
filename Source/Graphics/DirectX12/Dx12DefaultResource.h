#pragma once
#include "../DirectX12/Dx12GpuResource.h"
#include <span>
#include "../../../Include/Graphics/GraphicsCommon.h"
#include "../../../Include/Math/Vector2.h"
#include <cstdint>

struct ID3D12Device;
struct IDXGISwapChain4;

namespace Eugene
{
	class Dx12DefaultResource :
		public Dx12GpuResource
	{
	public:
		Dx12DefaultResource(ID3D12Device* device, size_t size);
		Dx12DefaultResource(IDXGISwapChain4* swapChain,std::uint32_t idx);
		Dx12DefaultResource(ID3D12Device* device,const Vector2& size, Format format, const std::span<float, 4>& clearColor);
		Dx12DefaultResource(ID3D12Device* device, const TextureInfo& formatData);
		Dx12DefaultResource(ID3D12Device* device,const Vector2& size, Format format, float clearValue = 1.0f);
	private:
	};
}
