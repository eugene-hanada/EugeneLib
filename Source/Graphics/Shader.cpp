#include "../../Include/Graphics/Shader.h"
#include <fstream>
#include "../../Include/Utils/EugeneLibException.h"

Eugene::Shader::Shader(const std::filesystem::path& path)
{
	std::ifstream file{ path , std::ios::binary};
	
	if (!file)
	{
		throw EugeneLibException{"ファイルを開けませんでした"};
	}
	code_.resize((std::filesystem::file_size(path) / 4ull) * 4ull);
	file.read(code_.data(), code_.size());
}

const char* Eugene::Shader::GetPtr(void) const
{
	return code_.data();
}

size_t Eugene::Shader::GetSize(void) const
{
	return code_.size();
}
