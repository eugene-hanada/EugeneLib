#pragma once
#include "../DirectX12/Dx12GpuResource.h"
#include <cstdint>

namespace EugeneLib
{
	class Dx12DefaultResource :
		public Dx12GpuResource
	{
	public:
		Dx12DefaultResource(size_t size, Graphics& graphics);
		Dx12DefaultResource(std::uint32_t idx, Graphics& graphics);
	private:
	};
}
