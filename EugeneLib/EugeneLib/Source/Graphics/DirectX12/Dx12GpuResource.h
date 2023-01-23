#pragma once
#include <memory>
#include <wrl.h>
#include "../../../Include/Graphics/GpuResource.h"

struct ID3D12Resource;

namespace Eugene
{
	class Dx12GpuResource :
		public GpuResource
	{
	public:
		virtual ~Dx12GpuResource();
	protected:
		void* GetResource(void) const;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	};
}
