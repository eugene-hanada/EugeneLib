#include "../../Include/Graphics/TextureLoader.h"
#include <unordered_map>
#include <functional>

#include "../ThirdParty/DitectXTex/DirectXTex.h"

#pragma comment(lib,"DitectXTex.lib")

std::unordered_map<std::string, std::function<void(const std::filesystem::path&)>> funcTbl
{
	{"png", [](const std::filesystem::path& path) {
		DirectX::TexMetadata* data{nullptr};
		DirectX::ScratchImage img;
		DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, data,img); }
	}
};


void EugeneLib::LoadTexture(const std::filesystem::path& path)
{
	auto exe = path.extension();
	DirectX::TexMetadata* data{ nullptr };
	DirectX::ScratchImage img;
	DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, data, img);

}
