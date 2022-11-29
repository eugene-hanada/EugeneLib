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
			PrimitiveType primitive ,
			bool isCulling ,
			ShaderLayoutSpan shaderLayout,
			SamplerSpan smplerLayout )
			;
	private:
		void* GetPipeline(void) const final;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	};
}
