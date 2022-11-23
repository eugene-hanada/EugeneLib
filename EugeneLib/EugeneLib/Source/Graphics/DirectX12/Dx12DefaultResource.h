#pragma once
#include "../DirectX12/Dx12GpuResource.h"
#include <span>
#include "../../../Include/Graphics/GraphicsCommon.h"
#include "../../../Include/Math/Vector2.h"
#include <cstdint>

namespace EugeneLib
{
	class Dx12DefaultResource :
		public Dx12GpuResource
	{
	public:
		Dx12DefaultResource(size_t size, Graphics& graphics);
		Dx12DefaultResource(std::uint32_t idx, Graphics& graphics);
		Dx12DefaultResource(const Vector2& size, Format format, const std::span<float, 4>& clearColor, Graphics& graphics);
	private:
	};
}
