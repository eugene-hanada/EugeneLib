#include "../../Include/Graphics/Shader.h"
#include <fstream>

EugeneLib::Shader::Shader(const std::filesystem::path& path)
{
	std::ifstream file{ path , std::ios::binary | std::ios::ate};
	
	if (!file)
	{
		return;
	}
	code_.resize(file.tellg());
	file.seekg(0);
	file.read(code_.data(), code_.size());
}

const char* EugeneLib::Shader::GetPtr(void) const
{
	return code_.data();
}

size_t EugeneLib::Shader::GetSize(void) const
{
	return code_.size();
}
