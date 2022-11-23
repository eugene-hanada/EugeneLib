#pragma once
#include <span>
#include "../../../Include/Graphics/GraphicsPipeline.h"

namespace EugeneLib
{
	class Dx12GraphicsPipeline :
		public GraphicsPipeline
	{
	public:
		Dx12GraphicsPipeline(std::span<ShaderInputLayout> layout, std::span<std::pair<Shader, ShaderType>> shaders);
	private:
	};
}
