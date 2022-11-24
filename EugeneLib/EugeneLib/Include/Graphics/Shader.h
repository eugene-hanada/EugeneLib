#pragma once
#include <vector>
#include <filesystem>

namespace EugeneLib
{
	class Shader
	{
	public:
		Shader(const std::filesystem::path& path);
		const char* GetPtr(void) const;
		size_t GetSize(void) const;
	private:
		std::vector<char> code_;
	};
}
