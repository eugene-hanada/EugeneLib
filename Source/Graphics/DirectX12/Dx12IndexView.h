#pragma once
#include "../../../Include/Graphics/IndexView.h"
#include "../../../Include/Graphics/GraphicsCommon.h"
#include <memory>

struct D3D12_INDEX_BUFFER_VIEW;

namespace Eugene
{
	class BufferResource;
	class Dx12IndexView :
		public IndexView
	{
	public:
		Dx12IndexView(std::uint32_t size, std::uint32_t indexNum, Format format, BufferResource& resource);
	private:
		void* GetView(void) final;

		/// <summary>
		/// インデックスバッファービュー
		/// </summary>
		std::unique_ptr< D3D12_INDEX_BUFFER_VIEW> bufferView_;
	};
}
