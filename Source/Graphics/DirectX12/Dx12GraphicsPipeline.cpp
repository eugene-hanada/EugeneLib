#include "Dx12GraphicsPipeline.h"
#include <d3d12.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Shader.h"
#include "Dx12Graphics.h"
#include "../../../Include/Common/EugeneLibException.h"

Eugene::Dx12GraphicsPipeline::Dx12GraphicsPipeline(
	ID3D12Device* device,
	ShaderInputSpan layout,
	ShaderTypePaisrSpan shaders,
	RenderTargetSpan rendertarges,
	TopologyType topologyType,
	bool isCulling,
	ShaderLayoutSpan shaderLayout,
	SamplerSpan samplerLayout
)
{
	std::vector<std::vector<CD3DX12_DESCRIPTOR_RANGE>> ranges(shaderLayout.size());
	for (std::uint64_t i = 0ull; i < ranges.size(); i++)
	{
		ranges[i].resize(shaderLayout[i].size());
		for (std::uint64_t j = 0ull; j < ranges[i].size(); j++)
		{
			ranges[i][j].Init(
				static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(shaderLayout[i][j].viewType_),
				shaderLayout[i][j].numView_,
				shaderLayout[i][j].baseRegister_
			);
		}
	}

	std::vector <CD3DX12_ROOT_PARAMETER> rootparam(ranges.size());
	for (std::uint64_t i = 0ull; i < rootparam.size(); i++)
	{
		rootparam[i].InitAsDescriptorTable(static_cast<std::uint32_t>(ranges[i].size()), ranges[i].data());
	}



	std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers(samplerLayout.size());
	for (std::uint64_t i = 0; i < samplers.size(); i++)
	{
		samplers[i].Init(
			static_cast<std::uint32_t>(i),
			static_cast<D3D12_FILTER>(samplerLayout[i].filter_),
			static_cast<D3D12_TEXTURE_ADDRESS_MODE>(samplerLayout[i].u_),
			static_cast<D3D12_TEXTURE_ADDRESS_MODE>(samplerLayout[i].v_)
		);
	}

	CD3DX12_ROOT_PARAMETER* paramP = nullptr;
	if (rootparam.size() > 0)
	{
		paramP = rootparam.data();
	}

	CD3DX12_STATIC_SAMPLER_DESC* samplP = nullptr;
	if (samplers.size() > 0)
	{
		samplP = samplers.data();
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(
		static_cast<std::uint32_t>(rootparam.size()),
		rootparam.data(),
		static_cast<std::uint32_t>(samplers.size()),
		samplP,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	if (FAILED(D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&rootSigBlob,
		&errorBlob)
	))
	{
		// throwかく
		throw EugeneLibException("ルードシグネチャ生成失敗");
	}

	if (FAILED(device->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(pipeline_.rootSignature_.ReleaseAndGetAddressOf())))
		)
	{
		throw EugeneLibException("ルードシグネチャ生成失敗");
	}


	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.pRootSignature = pipeline_.rootSignature_.Get();

	for (auto& shader : shaders)
	{
		switch (shader.second)
		{
			using enum ShaderType;
		case Pixel:
			gpipeline.PS.pShaderBytecode = shader.first.GetPtr();
			gpipeline.PS.BytecodeLength = shader.first.GetSize();
			break;
		case Vertex:
			gpipeline.VS.pShaderBytecode = shader.first.GetPtr();
			gpipeline.VS.BytecodeLength = shader.first.GetSize();
			break;
		default:
			break;
		}
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout(layout.size());
	for (std::uint64_t i = 0ull; i < layout.size(); i++)
	{
		inputLayout[i].SemanticName = layout[i].semanticName_;
		inputLayout[i].SemanticIndex = layout[i].semanticIdx_;
		inputLayout[i].Format = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(layout[i].format_)));
		inputLayout[i].InputSlot = layout[i].slot_;
		inputLayout[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputLayout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputLayout[i].InstanceDataStepRate = 0;
	}

	
	gpipeline.InputLayout.NumElements = static_cast<std::uint32_t>(inputLayout.size());
	gpipeline.InputLayout.pInputElementDescs = inputLayout.data();

	// デフォルトのサンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// ブレンド設定系
	gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	// トポロジー設定
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gpipeline.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(topologyType);
	gpipeline.NumRenderTargets = static_cast<std::uint32_t>(rendertarges.size());

	// ラスタライザ周りの設定
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	if (!isCulling)
	{
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;		// カリングをしない
	}

	// レンダーターゲット設定
	for (std::uint64_t i = 0; i < rendertarges.size(); i++)
	{
		gpipeline.RTVFormats[i] = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(rendertarges[i].format_)));
		switch (rendertarges[i].blendType_)
		{
			using enum BlendType;
		case Non:
			gpipeline.BlendState.RenderTarget[i].BlendEnable = false;
			break;
		case Alpha:
			gpipeline.BlendState.RenderTarget[i].BlendEnable = true;
			gpipeline.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			gpipeline.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			gpipeline.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			gpipeline.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
			gpipeline.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
			break;
		case Add:
			gpipeline.BlendState.RenderTarget[i].BlendEnable = true;
			gpipeline.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			gpipeline.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_COLOR;
			gpipeline.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_DEST_COLOR;
			gpipeline.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
			gpipeline.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
			break;
		case Sub:
			gpipeline.BlendState.RenderTarget[i].BlendEnable = true;
			gpipeline.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_SUBTRACT;
			gpipeline.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_COLOR;
			gpipeline.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_DEST_COLOR;
			gpipeline.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
			gpipeline.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
			break;
		default:
			break;
		}
	}

	// サンプリングに関する設定
	gpipeline.SampleDesc.Count = 1;
	gpipeline.SampleDesc.Quality = 0;

	gpipeline.DepthStencilState.DepthEnable = false;
	gpipeline.DepthStencilState.StencilEnable = false;

	if (FAILED(device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipeline_.state_.ReleaseAndGetAddressOf()))))
	{
		return;
	}
}

void* Eugene::Dx12GraphicsPipeline::GetPipeline(void)
{
	return (&pipeline_);
}
