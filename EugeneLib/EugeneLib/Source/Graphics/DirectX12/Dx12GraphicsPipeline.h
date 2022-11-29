#pragma once
#include "../../../Include/Graphics/GraphicsPipeline.h"
#include <wrl.h>

struct ID3D12RootSignature;
struct ID3D12PipelineState;

namespace EugeneLib
{
	
	class Dx12GraphicsPipeline :
		public GraphicsPipeline
	{
	public:
		Dx12GraphicsPipeline(
			Graphics& grahics,
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			PrimitiveType primitive = PrimitiveType::Triangle,
			bool isCulling = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan smplerLayout = SamplerSpan{})
			;
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	};
}
