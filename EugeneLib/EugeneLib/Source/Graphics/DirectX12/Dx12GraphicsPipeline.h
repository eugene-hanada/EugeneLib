#pragma once
#include "../../../Include/Graphics/GraphicsPipeline.h"

namespace EugeneLib
{
	class Dx12GraphicsPipeline :
		public GraphicsPipeline
	{
	public:
		Dx12GraphicsPipeline(
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			PrimitiveType primitive = PrimitiveType::Triangle,
			bool isCulling = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan smplerLayout = SamplerSpan{})
			;
	private:
	};
}
