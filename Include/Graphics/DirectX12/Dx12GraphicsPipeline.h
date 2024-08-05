﻿#pragma once
#include "../GraphicsPipeline.h"
#include <wrl.h>
#include "../../../Include/Utils/ArgsSpan.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;

namespace Eugene
{
	class ResourceBindLayout;

	class Pipeline
	{
	public:
		Pipeline() noexcept = default;


		struct PipeLineSet
		{
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
			Microsoft::WRL::ComPtr<ID3D12PipelineState> state_;
		};

		void* GetPipeline(void) noexcept
		{
			return &pipeline_;
		}

		void Final() noexcept
		{
			pipeline_.state_.Reset();
			pipeline_.rootSignature_.Reset();
		}

		Pipeline(const Pipeline& pipeline) noexcept
		{
			pipeline_.state_ = pipeline.pipeline_.state_;
			pipeline_.rootSignature_ = pipeline.pipeline_.rootSignature_;
		}

		Pipeline& operator=(const Pipeline& pipeline) noexcept
		{
			pipeline_.state_ = pipeline.pipeline_.state_;
			pipeline_.rootSignature_ = pipeline.pipeline_.rootSignature_;
		}

		Pipeline(Pipeline&& pipeline) noexcept
		{
			pipeline_.state_ = std::move(pipeline.pipeline_.state_);
			pipeline_.rootSignature_ = std::move(pipeline.pipeline_.rootSignature_);
		}

		Pipeline& operator=(Pipeline&& pipeline) noexcept
		{
			pipeline_.state_ = std::move(pipeline.pipeline_.state_);
			pipeline_.rootSignature_ = std::move(pipeline.pipeline_.rootSignature_);
		}
	private:



		Pipeline(
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType,
			bool isCulling,
			bool useDepth,
			std::uint8_t sampleCount
		);

		Pipeline(
			ResourceBindLayout& resourceBindLayout,
			const Shader& csShader
		);

		PipeLineSet pipeline_;

		friend class Graphics;
	};
}
