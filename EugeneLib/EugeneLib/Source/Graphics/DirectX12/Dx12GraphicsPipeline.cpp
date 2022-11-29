#include "Dx12GraphicsPipeline.h"
#include <d3d12.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Shader.h"
#include "../../../Include/Graphics/Graphics.h"

EugeneLib::Dx12GraphicsPipeline::Dx12GraphicsPipeline(
	Graphics& grahics,
	ShaderInputSpan layout,
	ShaderTypePaisrSpan shaders,
	RenderTargetSpan rendertarges,
	PrimitiveType primitive,
	bool isCulling,
	ShaderLayoutSpan shaderLayout,
	SamplerSpan samplerLayout
)
{
	std::vector<std::vector<CD3DX12_DESCRIPTOR_RANGE>> ranges(shaderLayout.size());
	for (size_t i = 0ull; i < ranges.size(); i++)
	{
		ranges[i].resize(shaderLayout[i].size());
		for (size_t j = 0ull; j < ranges[i].size(); j++)
		{
			ranges[i][j].Init(
				static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(shaderLayout[i][j].viewType_),
				shaderLayout[i][j].numView_,
				shaderLayout[i][j].baseRegister_
			);
		}
	}

	std::vector <CD3DX12_ROOT_PARAMETER> rootparam(ranges.size());
	for (size_t i = 0ull; i < rootparam.size(); i++)
	{
		rootparam[i].InitAsDescriptorTable(static_cast<std::uint32_t>(ranges[i].size()), ranges[i].data());
	}


	std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers(samplerLayout.size());
	for (size_t i = 0; i < samplers.size(); i++)
	{
		samplers[i].Init(
			static_cast<std::uint32_t>(i),
			static_cast<D3D12_FILTER>(samplerLayout[i].filter_),
			static_cast<D3D12_TEXTURE_ADDRESS_MODE>(samplerLayout[i].u_),
			static_cast<D3D12_TEXTURE_ADDRESS_MODE>(samplerLayout[i].v_)
		);
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(
		static_cast<std::uint32_t>(rootparam.size()),
		rootparam.data(), 
		static_cast<std::uint32_t>(samplers.size()),
		samplers.data(), 
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
		return ;
	}

	auto device = static_cast<ID3D12Device*>(grahics.GetDevice());
	if (FAILED(device->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSignature_.ReleaseAndGetAddressOf())))
		)
	{
		return ;
	}


	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline;
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
	for (size_t i = 0ull; i < layout.size(); i++)
	{
		inputLayout[i].SemanticName = layout[i].semanticName_;
		inputLayout[i].SemanticIndex = layout[i].semanticIdx_;
		inputLayout[i].Format = static_cast<DXGI_FORMAT>(layout[i].format_);
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

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gpipeline.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(primitive);
	gpipeline.NumRenderTargets = static_cast<std::uint32_t>(rendertarges.size());

	for (size_t i = 0; i < rendertarges.size(); i++)
	{
		gpipeline.RTVFormats[i] = static_cast<DXGI_FORMAT>(rendertarges[i].format_);
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

	if (FAILED(device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf()))))
	{

		return;
	}
}
