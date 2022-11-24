#include "Dx12GraphicsPipeline.h"
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Shader.h"

EugeneLib::Dx12GraphicsPipeline::Dx12GraphicsPipeline(ShaderInputSpan layout, ShaderTypePaisrSpan shaders, RenderTargetSpan rendertarges, PrimitiveType primitive, bool isCulling, ShaderLayoutSpan shaderLayout, SamplerSpan smplerLayout)
{
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

	// �f�t�H���g�̃T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// �u�����h�ݒ�n
	gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gpipeline.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(primitive);
	gpipeline.NumRenderTargets = rendertarges.size();

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
		default:
			break;
		}
	}

	// �T���v�����O�Ɋւ���ݒ�
	gpipeline.SampleDesc.Count = 1;
	gpipeline.SampleDesc.Quality = 0;


}
