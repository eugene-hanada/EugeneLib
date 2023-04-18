#pragma once
#include "../../../Include/Graphics/GraphicsPipeline.h"
#include <wrl.h>

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;

namespace Eugene
{
	
	class Dx12GraphicsPipeline :
		public GraphicsPipeline
	{
	public:
		Dx12GraphicsPipeline(
			ID3D12Device* device,
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			TopologyType topologyType,
			bool isCulling,
			ShaderLayoutSpan shaderLayout,
			SamplerSpan smplerLayout,
			bool useDepth
			);

		struct PipeLine
		{
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
			Microsoft::WRL::ComPtr<ID3D12PipelineState> state_;
		};
	private:
		void* GetPipeline(void) final;
		PipeLine pipeline_;

	};
}
