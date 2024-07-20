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

	class Dx12Pipeline :
		public Pipeline
	{
	public:

		Dx12Pipeline(
			ID3D12Device* device,
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType,
			bool isCulling,
			bool useDepth,
			std::uint8_t sampleCount
		);

		Dx12Pipeline(
			ID3D12Device* device,
			ResourceBindLayout& resourceBindLayout,
			const Shader& csShader
		);

		struct PipeLineSet
		{
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
			Microsoft::WRL::ComPtr<ID3D12PipelineState> state_;
		};
	private:
		void* GetPipeline(void) final;
		PipeLineSet pipeline_;

	};
}
