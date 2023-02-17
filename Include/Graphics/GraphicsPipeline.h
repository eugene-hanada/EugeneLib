#pragma once
#include <cstdint>
#include <span>
#include <vector>
#include "GraphicsCommon.h"
#include "Sampler.h"

namespace Eugene
{
	struct ShaderInputLayout
	{
		ShaderInputLayout();
		ShaderInputLayout(const char* semanticName, std::uint32_t semanticIdx, Format format, std::uint32_t slot = 0);
		const char* semanticName_;
		std::uint32_t semanticIdx_;
		Format format_;
		std::uint32_t slot_;
	};

	/// <summary>
	/// 
	/// </summary>
	enum class ShaderType
	{
		Vertex,
		Pixel
	};

	enum class BlendType
	{
		Non,
		Alpha,
		Add,
		Sub,
		Inv
	};

	struct RendertargetLayout
	{
		Format format_;
		BlendType blendType_;
	};

	enum class ViewType
	{
		Texture,
		UnoderedAccsec,
		ConstantBuffer,
		Sampler
	};

	struct ShaderLayout
	{
		ViewType viewType_;
		std::uint32_t numView_;
		std::uint32_t baseRegister_;
	};





	enum class TopologyType
	{
		Point = 1,
		Line = 2,
		Triangle = 3,
		Patch = 4
	};

	class Shader;
	class Graphics;
	using ShaderInputSpan = std::span<ShaderInputLayout>;
	using ShaderTypePaisrSpan = std::span<std::pair<Shader, ShaderType>>;
	using ShaderLayoutSpan = std::span<std::vector<ShaderLayout>>;
	using SamplerSpan = std::span<SamplerLayout>;
	using RenderTargetSpan = std::span <RendertargetLayout>;
	class GraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline();
		virtual void* GetPipeline(void) = 0;
	protected:
		GraphicsPipeline();

	};
}
