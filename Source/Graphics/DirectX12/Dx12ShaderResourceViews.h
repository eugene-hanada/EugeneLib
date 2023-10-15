#pragma once
#include "../../../Include/Graphics/ShaderResourceViews.h"
#include "Dx12Views.h"
#include "../../../Include/Common/ArgsSpan.h"

namespace Eugene
{
	class Dx12ShaderResourceViews :
		public Dx12Views, public ShaderResourceViews
	{
	public:
		Dx12ShaderResourceViews(ID3D12Device* device, std::uint64_t size);
		void CreateTexture(ImageResource& resource, std::uint64_t idx) final;
		void CreateConstantBuffer(BufferResource& resource, std::uint64_t idx) final;
		void CreateCubeMap(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) const final;
		std::uint64_t GetImg(void) final;
	protected:

		friend class Dx12ViewTable;
		friend class Dx12CommandList;
	};
}
