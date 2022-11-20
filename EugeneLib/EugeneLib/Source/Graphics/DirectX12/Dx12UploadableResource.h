#pragma once
#include "../DirectX12/Dx12GpuResource.h"

namespace EugeneLib
{
	class Dx12UploadableResource :
		public Dx12GpuResource
	{
	public:
		Dx12UploadableResource(size_t size, Graphics& graphics);
	private:
		void* Map(void) final;
		void UnMap(void) final;
	};
}

