#pragma once
#include <cstdint>
#include "GraphicsCommon.h"

namespace EugeneLib
{
	struct ShaderInputLayout
	{
		ShaderInputLayout(const char* semanticName, std::uint32_t semanticIdx, Format format, std::uint32_t slot = 0);
		const char* semanticName_;
		std::uint32_t semanticIdx_;
		Format format_;
		std::uint32_t slot_;
	};

	enum class ShaderType
	{
		Vertex,
		Pixel
	};



	class Shader;
	class GraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline();
		virtual void* GetPipeline(void) const = 0;
	protected:
		GraphicsPipeline();

	};
}
