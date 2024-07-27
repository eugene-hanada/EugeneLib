#include "Dx12GraphicsPipeline.h"
#include <d3d12.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Shader.h"
#include "Dx12Graphics.h"
#include "Dx12ResourceBindLayout.h"
#include "../../../Include/Utils/EugeneLibException.h"



Eugene::Dx12Pipeline::Dx12Pipeline(
	ID3D12Device* device, 
	ResourceBindLayout& resourceBindLayout, 
	const ArgsSpan<ShaderInputLayout>& layout,
	const ArgsSpan<ShaderPair>& shaders,
	const ArgsSpan<RendertargetLayout>& rendertarges,
	TopologyType topologyType, 
	bool isCulling, bool useDepth,
	std::uint8_t sampleCount)
{
	pipeline_.rootSignature_ = static_cast<Dx12ResourceBindLayout&>(resourceBindLayout).rootSignature_;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	// ルードシグネチャをセット
	gpipeline.pRootSignature = pipeline_.rootSignature_.Get();
	
	// 各シェーダーをセット
	for (std::uint64_t i = 0ull; i < shaders.size(); i++)
	{
		auto& shader = shaders.at(i);
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
		case Geometry:
			gpipeline.GS.pShaderBytecode = shader.first.GetPtr();
			gpipeline.GS.BytecodeLength = shader.first.GetSize();
			break;
		case Domain:
			gpipeline.DS.pShaderBytecode = shader.first.GetPtr();
			gpipeline.DS.BytecodeLength = shader.first.GetSize();
			break;
		case Hull:
			gpipeline.HS.pShaderBytecode = shader.first.GetPtr();
			gpipeline.HS.BytecodeLength = shader.first.GetSize();
			break;
		default:
			break;
		}
	}

	// シェーダーの入力のレイアウトをセットする
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout(layout.size());
	for (std::uint64_t i = 0ull; i < layout.size(); i++)
	{
		auto tmpFormat = layout.at(i).format_;
		if (tmpFormat == Format::AUTO_BACKBUFFER)
		{
			tmpFormat = Dx12Graphics::BackBufferFormat();
		}
		inputLayout[i].SemanticName = layout.at(i).semanticName_;
		inputLayout[i].SemanticIndex = layout.at(i).semanticIdx_;
		inputLayout[i].Format = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<size_t>(tmpFormat)));
		inputLayout[i].InputSlot = layout.at(i).slot_;
		inputLayout[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputLayout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputLayout[i].InstanceDataStepRate = 0;
	}
	gpipeline.InputLayout.NumElements = static_cast<std::uint32_t>(inputLayout.size());
	gpipeline.InputLayout.pInputElementDescs = inputLayout.data();

	// デフォルトのサンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// ブレンド設定をいったんデフォルトで初期化する
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
		auto tmpFormat = rendertarges.at(i).format_;
		if (tmpFormat == Format::AUTO_BACKBUFFER)
		{
			tmpFormat = Dx12Graphics::BackBufferFormat();
		}
		gpipeline.RTVFormats[i] = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(tmpFormat)));
		switch (rendertarges.at(i).blendType_)
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
		case Inv:
			gpipeline.BlendState.RenderTarget[i].BlendEnable = true;
			gpipeline.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			gpipeline.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_INV_SRC_COLOR;
			gpipeline.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_DEST_COLOR;
			gpipeline.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
			gpipeline.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
			break;
		default:
			break;
		}
	}

	// サンプリングに関する設定
	gpipeline.SampleDesc.Count = sampleCount;
	gpipeline.SampleDesc.Quality = 0;

	// デプスバッファ周りの設定
	gpipeline.DepthStencilState.StencilEnable = false;
	gpipeline.DepthStencilState.DepthEnable = useDepth;
	if (useDepth)
	{
		CD3DX12_DEPTH_STENCIL_DESC depthDesc(CD3DX12_DEFAULT{});
		gpipeline.DepthStencilState = depthDesc;
	}

	if (FAILED(device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipeline_.state_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException{ "グラフィックスパイプラインステート生成失敗" };
	}
}

Eugene::Dx12Pipeline::Dx12Pipeline(ID3D12Device* device, ResourceBindLayout& resourceBindLayout, const Shader& csShader)
{
	//D3D12_DESCRIPTOR_RANGE range[1] = {};
	//range[0].NumDescriptors = 1;//1つ 
	//range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;//u 
	//range[0].BaseShaderRegister = 0;//u0 
	//range[0].OffsetInDescriptorsFromTableStart = 0;
	//range[0].RegisterSpace = 0;

	//D3D12_ROOT_PARAMETER rp[1] = {};
	//rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//rp[0].DescriptorTable.NumDescriptorRanges = 1;
	//rp[0].DescriptorTable.pDescriptorRanges = range;

	//D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	//rootSigDesc.NumParameters = 1;

	//rootSigDesc.pParameters = rp;
	//rootSigDesc.NumStaticSamplers = 0;
	//rootSigDesc.pStaticSamplers = nullptr;
	//rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
	//ID3DBlob* errBlob = nullptr;

	//ID3DBlob* rootSigBlob = nullptr;
	//D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob,
	//	&errBlob);
	//ID3D12RootSignature* rootSignature = nullptr;
	//auto result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(),
	//	rootSigBlob->GetBufferSize(), IID_PPV_ARGS(pipeline_.rootSignature_.ReleaseAndGetAddressOf()));

	pipeline_.rootSignature_ = static_cast<Dx12ResourceBindLayout&>(resourceBindLayout).rootSignature_;

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineDesc{};
	computePipelineDesc.CS.pShaderBytecode = csShader.GetPtr();
	computePipelineDesc.CS.BytecodeLength = csShader.GetSize();
	computePipelineDesc.NodeMask = 0;
	computePipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	computePipelineDesc.pRootSignature = pipeline_.rootSignature_.Get();
	auto result = device->CreateComputePipelineState(&computePipelineDesc, IID_PPV_ARGS(pipeline_.state_.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		throw CreateErrorException{ "コンピュートパイプラインステート生成失敗" };
	}
}

void* Eugene::Dx12Pipeline::GetPipeline(void)
{
	return (&pipeline_);
}
