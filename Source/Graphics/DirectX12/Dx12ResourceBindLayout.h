#pragma once
#include "../../../Include/Graphics/ResourceBindLayout.h"
#include "../../../Include/Common/ArgsSpan.h"
#include <wrl.h>

struct ID3D12RootSignature;
struct ID3D12Device;

namespace Eugene
{
	class Dx12ResourceBindLayout :
		public ResourceBindLayout
	{
	public:

		Dx12ResourceBindLayout(ID3D12Device* device, const ArgsSpan<ArgsSpan<Bind>>& viewTypes);
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

		friend class Dx12GraphicsPipeline;
	};
}
