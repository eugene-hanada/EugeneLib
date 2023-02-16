#pragma once
#include "../../../Include/Graphics/IndexView.h"
#include "../../../Include/Graphics/GraphicsCommon.h"
#include <memory>

struct D3D12_INDEX_BUFFER_VIEW;

namespace Eugene
{
	class GpuResource;
	class Dx12IndexView :
		public IndexView
	{
	public:
		Dx12IndexView(std::uint64_t size, Format format, GpuResource& resource);
	private:
		void* GetView(void) const final;
		std::unique_ptr< D3D12_INDEX_BUFFER_VIEW> bufferView_;
	};
}
