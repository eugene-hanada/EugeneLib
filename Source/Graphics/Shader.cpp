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
	byteCode_.Resize((std::filesystem::file_size(path) / 4ull) * 4ull);
	file.read(reinterpret_cast<char*>(byteCode_.data()), byteCode_.size());
}

Eugene::Shader::Shader(std::span<const std::uint8_t> byteCode) :
	byteCode_{(byteCode.size() / 4ull) * 4ull}
{
	std::copy_n(byteCode.begin(), byteCode_.size(), byteCode_.begin());
}

const char* Eugene::Shader::GetPtr(void) const noexcept
{
	return reinterpret_cast<const char*>(byteCode_.data());
}

std::uint64_t Eugene::Shader::GetSize(void) const noexcept
{
	return byteCode_.size();
}
