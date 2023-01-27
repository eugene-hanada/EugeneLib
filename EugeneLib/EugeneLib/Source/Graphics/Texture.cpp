#include "../../Include/Graphics/Texture.h"
#include <unordered_map>
#include <functional>
#include "../../DirectXTex/DirectXTex/DirectXTex.h"
#include "../../Include/Common/EugeneLibException.h"

struct Load
{
	bool operator()(const std::filesystem::path& path, Eugene::TextureInfo& info, std::vector<std::vector<std::uint8_t>>& data )
	{
		DirectX::TexMetadata meta{};
		DirectX::ScratchImage img{};
		if (FAILED(DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, &meta, img)))
		{
			return false;
		}
		auto rowData = img.GetImage(0, 0, 0);
		data.resize(1);
		data[0].resize(rowData->slicePitch);

		info = {
			meta.width,
			meta.height,
			static_cast<Eugene::Format>(meta.format),
			meta.arraySize,
			static_cast<std::uint16_t>(meta.mipLevels)
		};

		std::copy_n(rowData->pixels, data[0].size(), data[0].data());

		return true;
	}
};

struct LoadDss
{
	bool operator()(const std::filesystem::path& path, Eugene::TextureInfo& info, std::vector<std::vector<std::uint8_t>>& data)
	{
		DirectX::TexMetadata meta{};
		DirectX::ScratchImage img{};
		DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, &meta, img);
		info = {
			meta.width,
			meta.height,
			static_cast<Eugene::Format>(meta.format),
			meta.arraySize,
			static_cast<std::uint16_t>(meta.mipLevels)
		};
		
	}
};


std::unordered_map<std::string, std::function<bool(const std::filesystem::path&, Eugene::TextureInfo&, std::vector<std::vector<std::uint8_t>>&)>> funcTbl
{
	{".png",Load()},
	{".jpg",Load()},
};

Eugene::Texture::Texture(const std::filesystem::path& path)
{
	auto exe = path.extension();

	if (!funcTbl.contains(exe.string()))
	{
		exe = ".bmp";
	}

	if (!funcTbl[exe.string()](path, info_, data_))
	{
		// 
		throw EugeneLibException("テクスチャファイル読み込みに失敗" + path.string());
	}
}

const Eugene::TextureInfo& Eugene::Texture::GetInfo(void) const&
{
	return info_;
}

const std::vector<std::uint8_t>& Eugene::Texture::GetData(void) const&
{
	return data_[0];
}

std::uint8_t* Eugene::Texture::GetData(void)&
{
	return data_[0].data();
}
