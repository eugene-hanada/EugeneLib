#include "../../../Include/Graphics/DirectX12/Dx12ResourceBindLayout.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include <vector>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Utils/EugeneLibException.h"


Eugene::ResourceBindLayout::ResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes,ResourceBindFlags flags)
{
	auto b = viewTypes.begin();
	auto size = viewTypes.size();

	std::uint32_t texRegister = 0u;
	std::uint32_t cbRegister = 0u;
	std::uint32_t uaRegister = 0u;
	std::uint32_t smpRegister = 0u;

	std::vector<std::vector<CD3DX12_DESCRIPTOR_RANGE>> range(size);
	std::vector <CD3DX12_ROOT_PARAMETER> rootparam(size);
	
	for (std::uint64_t i = 0ull; i < size; i++)
	{
		auto now = (b + i);
		range[i].resize(now->size());
		for (std::uint64_t j = 0ull; j < now->size(); j++)
		{
			auto bind = now->begin() + j;
			std::uint32_t nowRegister = 0u;

			if (bind->viewType_ == ViewType::Constans)
			{
				range[i].clear();
				rootparam[i].InitAsConstants(bind->viewNum_, cbRegister++);
				break;
			}

			switch (bind->viewType_)
			{
			case ViewType::Texture:
				nowRegister = texRegister++;
				break;
			case ViewType::UnoderedAccess:
				nowRegister = uaRegister++;
				break;
			case ViewType::ConstantBuffer:
				nowRegister = cbRegister++;
				break;
			case ViewType::Sampler:
				nowRegister = smpRegister++;
				break;
			default:
				break;
			}

			range[i][j].Init(
				static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(bind->viewType_),
				bind->viewNum_,
				nowRegister
			);
		}
	}

	
	for (std::uint64_t i = 0ull; i < rootparam.size(); i++)
	{
		if (range[i].empty())
		{
			continue;
		}
		rootparam[i].InitAsDescriptorTable(static_cast<std::uint32_t>(range[i].size()), range[i].data());
	}

	constexpr D3D12_ROOT_SIGNATURE_FLAGS toRootSignatureFlag[]
	{
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT,
	};
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags{ D3D12_ROOT_SIGNATURE_FLAG_NONE };

	for (std::size_t i = 0; i < flags.size(); i++)
	{
		if (flags.test(i))
		{
			rootSignatureFlags |= toRootSignatureFlag[i];
		}
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(
		static_cast<std::uint32_t>(rootparam.size()),
		rootparam.data(),
		0u,
		nullptr,
		rootSignatureFlags
	);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	if (FAILED(D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob)
	))
	{
		// throwかく
		throw EugeneLibException("ルードシグネチャ生成失敗");
	}

	if (FAILED(Graphics::GetInstance().device_->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSignature_.ReleaseAndGetAddressOf())))
		)
	{
		throw EugeneLibException("ルードシグネチャ生成失敗");
	}
}
