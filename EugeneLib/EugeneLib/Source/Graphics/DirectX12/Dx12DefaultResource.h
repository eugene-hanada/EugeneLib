#pragma once
#include "../DirectX12/Dx12GpuResource.h"
#include <span>
#include "../../../Include/Graphics/GraphicsCommon.h"
#include "../../../Include/Math/Vector2.h"
#include <cstdint>

struct ID3D12Device;
struct IDXGISwapChain4;

namespace EugeneLib
{
	class Dx12DefaultResource :
		public Dx12GpuResource
	{
	public:
		Dx12DefaultResource(size_t size, ID3D12Device* device);
		Dx12DefaultResource(std::uint32_t idx, IDXGISwapChain4* swapChain);
		Dx12DefaultResource(const Vector2& size, Format format, const std::span<float, 4>& clearColor, ID3D12Device* device);
		Dx12DefaultResource(const TextureInfo& formatData, ID3D12Device* device);
	private:
	};
}
