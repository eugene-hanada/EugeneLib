#include "../../../Include/Graphics/DirectX12/Dx12GraphicsPipeline.h"
#include <d3d12.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Shader.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include "../../../Include/Graphics/DirectX12/Dx12ResourceBindLayout.h"
#include "../../../Include/Debug/Debug.h"



Eugene::Pipeline::Pipeline(
	ResourceBindLayout& resourceBindLayout,
	const ArgsSpan<ShaderInputLayout>& layout,
	const ArgsSpan<ShaderPair>& shaders,
	const ArgsSpan<RendertargetLayout>& rendertarges,
	TopologyType topologyType,
	bool isCulling,
	const DepthStencilLayout& depthStencil,
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
			gpipeline.PS.pShaderBytecode = shader.first.data();
			gpipeline.PS.BytecodeLength = shader.first.size();
			break;
		case Vertex:
			gpipeline.VS.pShaderBytecode = shader.first.data();
			gpipeline.VS.BytecodeLength = shader.first.size();
			break;
		case Geometry:
			gpipeline.GS.pShaderBytecode = shader.first.data();
			gpipeline.GS.BytecodeLength = shader.first.size();
			break;
		case Domain:
			gpipeline.DS.pShaderBytecode = shader.first.data();
			gpipeline.DS.BytecodeLength = shader.first.size();
			break;
		case Hull:
			gpipeline.HS.pShaderBytecode = shader.first.data();
			gpipeline.HS.BytecodeLength = shader.first.size();
			break;
		default:
			break;
		}
	}

	// シェーダーの入力のレイアウトをセットする
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout(layout.size());
	for (std::uint64_t i = 0ull; i < layout.size(); i++)
	{
		auto tmpFormat = layout.at(i).format;
		if (tmpFormat == Format::AUTO_BACKBUFFER)
		{
			tmpFormat = Graphics::BackBufferFormat();
		}
		inputLayout[i].SemanticName = layout.at(i).semanticName;
		inputLayout[i].SemanticIndex = layout.at(i).semanticIdx;
		inputLayout[i].Format = Graphics::FormatToBackEndFormat(tmpFormat);
		inputLayout[i].InputSlot = layout.at(i).slot;
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

	constexpr D3D12_BLEND_OP toDx12BlentOP[] = {
		D3D12_BLEND_OP_ADD,
		D3D12_BLEND_OP_SUBTRACT,
		D3D12_BLEND_OP_REV_SUBTRACT,
		D3D12_BLEND_OP_MIN,
		D3D12_BLEND_OP_MAX
	};

	constexpr D3D12_BLEND toDx12BlendFactor[] = {
		D3D12_BLEND_ZERO,
		D3D12_BLEND_ONE,
		D3D12_BLEND_SRC_COLOR,
		D3D12_BLEND_INV_SRC_COLOR,
		D3D12_BLEND_SRC_ALPHA,
		D3D12_BLEND_INV_SRC_ALPHA,
		D3D12_BLEND_DEST_COLOR,
		D3D12_BLEND_INV_DEST_COLOR,
		D3D12_BLEND_DEST_ALPHA,
		D3D12_BLEND_INV_DEST_ALPHA
	};


	constexpr D3D12_COMPARISON_FUNC toDx12CompareFunc[] = {
		D3D12_COMPARISON_FUNC_NEVER,
		D3D12_COMPARISON_FUNC_LESS,
		D3D12_COMPARISON_FUNC_EQUAL,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_COMPARISON_FUNC_GREATER,
		D3D12_COMPARISON_FUNC_NOT_EQUAL,
		D3D12_COMPARISON_FUNC_GREATER_EQUAL,
		D3D12_COMPARISON_FUNC_ALWAYS
	};

	constexpr D3D12_STENCIL_OP toDx12StencilOp[] = {
		D3D12_STENCIL_OP_KEEP,
		D3D12_STENCIL_OP_ZERO,
		D3D12_STENCIL_OP_REPLACE,
		D3D12_STENCIL_OP_INCR_SAT,
		D3D12_STENCIL_OP_DECR_SAT,
		D3D12_STENCIL_OP_INVERT,
		D3D12_STENCIL_OP_INCR,
		D3D12_STENCIL_OP_DECR
	};

	// レンダーターゲット設定
	for (std::uint64_t i = 0; i < std::size(gpipeline.RTVFormats); i++)
	{
		if (i >= rendertarges.size())
		{
			gpipeline.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;	// レンダーターゲットがない場合はUNKNOWNをセット
			continue;
		}
		auto tmpFormat = rendertarges.at(i).format;
		if (tmpFormat == Format::AUTO_BACKBUFFER)
		{
			tmpFormat = Graphics::BackBufferFormat();
		}
		gpipeline.RTVFormats[i] = Graphics::FormatToBackEndFormat(tmpFormat);
		auto tmpState = rendertarges.at(i).blendState;
 		gpipeline.BlendState.RenderTarget[i].BlendEnable =  rendertarges.at(i).blendState.enable;
		gpipeline.BlendState.RenderTarget[i].SrcBlend = toDx12BlendFactor[static_cast<std::uint8_t>(rendertarges.at(i).blendState.srcColor)];
		gpipeline.BlendState.RenderTarget[i].DestBlend = toDx12BlendFactor[static_cast<std::uint8_t>(rendertarges.at(i).blendState.dstColor)];
		gpipeline.BlendState.RenderTarget[i].BlendOp = toDx12BlentOP[static_cast<std::uint8_t>(rendertarges.at(i).blendState.colorOp)];
		gpipeline.BlendState.RenderTarget[i].SrcBlendAlpha = toDx12BlendFactor[static_cast<std::uint8_t>(rendertarges.at(i).blendState.srcAlpha)];
		gpipeline.BlendState.RenderTarget[i].DestBlendAlpha = toDx12BlendFactor[static_cast<std::uint8_t>(rendertarges.at(i).blendState.dstAlpha)];
		gpipeline.BlendState.RenderTarget[i].RenderTargetWriteMask = rendertarges.at(i).blendState.writeMask;
	}
	gpipeline.NumRenderTargets = static_cast<std::uint32_t>(rendertarges.size());	// レンダーターゲットの数をセット

	// サンプリングに関する設定
	gpipeline.SampleDesc.Count = sampleCount;
	gpipeline.SampleDesc.Quality = 0;

	// デプスバッファ周りの設定
	gpipeline.DepthStencilState.StencilEnable = depthStencil.stencilState.enable;
	gpipeline.DepthStencilState.StencilReadMask = depthStencil.stencilState.readMask;
	gpipeline.DepthStencilState.StencilWriteMask = depthStencil.stencilState.writeMask;
	gpipeline.DepthStencilState.DepthEnable = depthStencil.depthState.enable;
	gpipeline.DepthStencilState.DepthWriteMask = depthStencil.depthState.writeEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	gpipeline.DepthStencilState.DepthFunc = toDx12CompareFunc[static_cast<std::uint8_t>(depthStencil.depthState.func)];
	gpipeline.DSVFormat = Graphics::FormatToBackEndFormat(depthStencil.format);
	gpipeline.DepthStencilState.FrontFace = D3D12_DEPTH_STENCILOP_DESC{
		
		toDx12StencilOp[static_cast<std::uint8_t>(depthStencil.stencilState.frontState.failOp)],
		toDx12StencilOp[static_cast<std::uint8_t>(depthStencil.stencilState.frontState.depthFailOp)],
		toDx12StencilOp[static_cast<std::uint8_t>(depthStencil.stencilState.frontState.passOp)],
		toDx12CompareFunc[static_cast<std::uint8_t>(depthStencil.stencilState.frontState.func)],
	};
	gpipeline.DepthStencilState.BackFace = D3D12_DEPTH_STENCILOP_DESC{
		toDx12StencilOp[static_cast<std::uint8_t>(depthStencil.stencilState.backState.failOp)],
		toDx12StencilOp[static_cast<std::uint8_t>(depthStencil.stencilState.backState.depthFailOp)],
		toDx12StencilOp[static_cast<std::uint8_t>(depthStencil.stencilState.backState.passOp)],
		toDx12CompareFunc[static_cast<std::uint8_t>(depthStencil.stencilState.backState.func)],
	};
	
	auto result = Graphics::GetInstance().device_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipeline_.state_.ReleaseAndGetAddressOf()));
	EUGENE_RELEASE_ASSERT_MSG(SUCCEEDED(result), "グラフィックスパイプラインステート生成失敗");
}

Eugene::Pipeline::Pipeline(ResourceBindLayout& resourceBindLayout, const std::span<const std::uint8_t> csShader)
{
	pipeline_.rootSignature_ = resourceBindLayout.rootSignature_;

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineDesc{};
	computePipelineDesc.CS.pShaderBytecode = csShader.data();
	computePipelineDesc.CS.BytecodeLength = csShader.size();
	computePipelineDesc.NodeMask = 0;
	computePipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	computePipelineDesc.pRootSignature = pipeline_.rootSignature_.Get();
	auto result = Graphics::GetInstance().device_->CreateComputePipelineState(&computePipelineDesc, IID_PPV_ARGS(pipeline_.state_.ReleaseAndGetAddressOf()));
	EUGENE_RELEASE_ASSERT_MSG(SUCCEEDED(result), "コンピュートパイプラインステート生成失敗");
}

