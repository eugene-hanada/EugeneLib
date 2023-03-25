#include "../../Include/Graphics/Image.h"
#include "../../Include/Common/EugeneLibException.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_image.h"

#include "DdsLoad.h"

const Eugene::Image::LoadFuncMap Eugene::Image::loadFuncMap_{
	{".png",&Image::LoadStb},
	{".jpeg",&Image::LoadStb},
	{".tga",&Image::LoadStb},
	{".bmp",&Image::LoadStb},
	{".psd",&Image::LoadStb},
	{".gif",&Image::LoadStb},
	{".hdr",&Image::LoadStb},
	{".pic",&Image::LoadStb},
	{".pnm",&Image::LoadStb},
	{".dds",&Image::LoadDds}
};

constexpr int ddsSig = std::endian::native == std::endian::little ? 542327876 : 7678324205;

Eugene::Image::Image(const std::filesystem::path& path) :
	info_{}
{
	auto ext = path.extension().string();
	if (!loadFuncMap_.contains(ext))
	{
		throw EugeneLibException{ "対応していないファイルです" };
	}

	if (!(this->*loadFuncMap_.at(ext))(path))
	{
		throw EugeneLibException{ "ファイルロードエラー" };
	}
}

const Eugene::TextureInfo& Eugene::Image::GetInfo(void) const&
{
	return info_;
}


std::uint8_t* Eugene::Image::GetData(std::uint32_t arrayIndex, std::uint16_t mipMapLevel)
{
	return data_.at((arrayIndex * info_.mipLevels) + mipMapLevel).data();
}

bool Eugene::Image::LoadStb(const std::filesystem::path& path)
{
	int w, h, c;
	auto img = stbi_load(path.string().c_str(), &w, &h, &c, STBI_default);

	if (img == nullptr)
	{
		return false;
	}

	data_.resize(1);
	data_[0].resize(w * h * c);
	std::copy_n(img, data_[0].size(), data_[0].data());
	info_.arraySize = 1;
	info_.mipLevels = 1;
	info_.format = Format::R8G8B8A8_UNORM;
	info_.width = w;
	info_.height = h;
	stbi_image_free(img);

	return true;
}

bool Eugene::Image::LoadDds(const std::filesystem::path& path)
{
	std::ifstream file(path, std::ios::binary);
	int sig;
	
	// シグネチャ読み込み
	file.read(reinterpret_cast<char*>(&sig), 4);
	if (sig != ddsSig)
	{
		// シグネチャがあってなかったら読み込みをやめる
		return false;
	}

	// ヘッダー読み込み
	DdsHeader h;
	file.read(reinterpret_cast<char*>(&h), sizeof(h));

	// 情報をセット
	info_.arraySize = 1;
	info_.mipLevels = h.mipMapCount;
	info_.height = h.height;
	info_.width = h.width;
	
	// fourCCをチェック
	switch (h.fourCC)
	{
	case '01XD':
		// DX10の時
		LoadDdsExtension(file, info_);
		break;
	case '1TXD':
		// DXT1の時
		info_.format = Format::BC1_UNORM;
		break;
	case '3TXD':
		// DXT3の時
		info_.format = Format::BC2_UNORM;
		break;
	case '5TXD':
		// DXT5の時
		info_.format = Format::BC3_UNORM;
		break;
	default:
		// それ以外の時とりあえずRGBA8ビットのやつにしとく
		info_.format = Format::R8G8B8A8_UNORM;
		break;
	}

	// キューブマップか調べる
	constexpr auto cubeMapFlagBit = 0x200 | 0x400 | 0x800 | 0x1000 | 0x2000 | 0x4000 | 0x8000;
	if ((cubeMapFlagBit & h.caps2) != 0)
	{
		info_.arraySize = 6;
	}

	auto pixelPerPite = h.rgbBitCount / 8;
	data_.resize(info_.arraySize * info_.mipLevels);
	
	// 配列サイズとミップマップの分を読み込む
	for (int j = 0; j < info_.arraySize; j++)
	{
		for (int i = 0; i < info_.mipLevels; i++)
		{
			// インデックス
			auto idx = j * info_.mipLevels + i;

			// サイズを計算
			auto size = colcMap.at(h.fourCC)(std::max(1, h.width >> i), std::max(1, h.height >> i), pixelPerPite);
			
			// リサイズして読み込む
			data_[idx].resize(size);
			file.read(reinterpret_cast<char*>(data_[idx].data()), size);
		}
	}
	return true;
}
