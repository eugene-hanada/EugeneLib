#pragma once
#include "../../../Include/Graphics/GraphicsPipeline.h"
#include <wrl.h>
#include "../../../Include/Utils/ArgsSpan.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;

namespace Eugene
{
	class ResourceBindLayout;

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

		Dx12GraphicsPipeline(
			ID3D12Device* device,
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType,
			bool isCulling,
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
