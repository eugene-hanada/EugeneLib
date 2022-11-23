#pragma once
#include <vector>
#include <filesystem>

namespace EugeneLib
{
	class Shader
	{
	public:
		Shader(const std::filesystem::path& path);
	private:
		std::vector<char> code_;
	};
}
