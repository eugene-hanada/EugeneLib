#pragma once
#include <filesystem>
#include <vector>
#include "GraphicsCommon.h"

namespace EugeneLib
{

	class Texture
	{
	public:
		Texture(const std::filesystem::path& path);
		const TextureInfo& GetInfo(void) const&;
		const std::vector<std::uint8_t>& GetData(void) const&;
		std::uint8_t* GetData(void)&;
	private:
		std::vector<std::vector<std::uint8_t>> data_;
		TextureInfo info_;
	};
}
