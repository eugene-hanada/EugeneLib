#include "../../../Include/Graphics/DirectX12/Dx12GraphicsPipeline.h"
#include <d3d12.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Shader.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include "../../../Include/Graphics/DirectX12/Dx12ResourceBindLayout.h"
#include "../../../Include/Utils/EugeneLibException.h"



Eugene::Pipeline::Pipeline(
	ResourceBindLayout& resourceBindLayout, 
	const ArgsSpan<ShaderInputLayout>& layout,
	const ArgsSpan<ShaderPair>& shaders,
	const ArgsSpan<RendertargetLayout>& renderTargets,
	TopologyType topologyType, 
	bool isCulling, bool useDepth,
	std::uint8_t sampleCount)
{
	pipeline_.rootSignature_ = resourceBindLayout.rootSignature_;

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
			tmpFormat = Graphics::BackBufferFormat();
		}
		inputLayout[i].SemanticName = layout.at(i).semanticName_;
		inputLayout[i].SemanticIndex = layout.at(i).semanticIdx_;
		inputLayout[i].Format = static_cast<DXGI_FORMAT>(Graphics::FormatToDxgiFormat_.at(static_cast<size_t>(tmpFormat)));
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
	gpipeline.NumRenderTargets = static_cast<std::uint32_t>(renderTargets.size());

	// ラスタライザ周りの設定
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	if (!isCulling)
	{
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;		// カリングをしない
	}

	// レンダーターゲット設定
	for (std::uint64_t i = 0; i < renderTargets.size(); i++)
	{
		auto tmpFormat = renderTargets.at(i).format_;
		if (tmpFormat == Format::AUTO_BACKBUFFER)
		{
			tmpFormat = Graphics::BackBufferFormat();
		}
		gpipeline.RTVFormats[i] = static_cast<DXGI_FORMAT>(Graphics::FormatToDxgiFormat_.at(static_cast<int>(tmpFormat)));
		switch (renderTargets.at(i).blendType_)
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

	if (FAILED(Graphics::GetInstance().device_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipeline_.state_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException{ "グラフィックスパイプラインステート生成失敗" };
	}
}

Eugene::Pipeline::Pipeline(ResourceBindLayout& resourceBindLayout, const Shader& csShader)
{
	pipeline_.rootSignature_ = resourceBindLayout.rootSignature_;

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineDesc{};
	computePipelineDesc.CS.pShaderBytecode = csShader.GetPtr();
	computePipelineDesc.CS.BytecodeLength = csShader.GetSize();
	computePipelineDesc.NodeMask = 0;
	computePipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	computePipelineDesc.pRootSignature = pipeline_.rootSignature_.Get();
	auto result = Graphics::GetInstance().device_->CreateComputePipelineState(&computePipelineDesc, IID_PPV_ARGS(pipeline_.state_.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		throw EugeneLibException{ "コンピュートパイプラインステート生成失敗" };
	}
}

