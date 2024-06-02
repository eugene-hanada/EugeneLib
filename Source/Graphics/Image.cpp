#include "../../Include/Graphics/Image.h"
#include "../../Include/Utils/EugeneLibException.h"
#include <fstream>
#include <bit>

#define STB_IMAGE_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_image.h"

#include "DdsLoad.h"

const Eugene::Image::LoadFromFileFuncMap Eugene::Image::loadFromFileFuncMap_
{
	{std::hash<std::string_view>()(".png"),&Image::LoadStbFromFile},
	{std::hash<std::string_view>()(".jpeg"),&Image::LoadStbFromFile},
	{std::hash<std::string_view>()(".tga"),&Image::LoadStbFromFile},
	{std::hash<std::string_view>()(".bmp"),&Image::LoadStbFromFile},
	{std::hash<std::string_view>()(".psd"),&Image::LoadStbFromFile},
	{std::hash<std::string_view>()(".gif"),&Image::LoadStbFromFile},
	{std::hash<std::string_view>()(".hdr"),&Image::LoadStbFromFile},
	{std::hash<std::string_view>()(".pic"),&Image::LoadStbFromFile},
	{std::hash<std::string_view>()(".pnm"),&Image::LoadStbFromFile},
	{std::hash<std::string_view>()(".dds"),&Image::LoadDdsFromFile}
};

constexpr int ddsSig = std::endian::native == std::endian::little ? 542327876 : 7678324205;

Eugene::Image::Image(const std::filesystem::path& path) :
	info_{}
{
	auto tmp = std::hash<std::string_view>()(path.extension().string());
	if (!(this->*loadFromFileFuncMap_.at(tmp))(path))
	{
		throw EugeneLibException{ "ファイル読み込み失敗" };
	}
}

Eugene::Image::Image(std::span<std::byte> data, const std::string_view& ext):
	info_{}
{
	auto tmp = std::hash<std::string_view>()(ext);
}

const Eugene::TextureInfo& Eugene::Image::GetInfo(void) const&
{
	return info_;
}


std::uint8_t* Eugene::Image::GetData(std::uint32_t arrayIndex, std::uint16_t mipMapLevel)
{
	return data_.at((arrayIndex * info_.mipLevels) + mipMapLevel).data();
}


Eugene::Image::Image(Image&& img) noexcept:
	info_{img.info_}
{
	data_ = std::move(img.data_);
}

Eugene::Image& Eugene::Image::operator=(Image&& img) noexcept
{
	data_ = std::move(img.data_);
	info_ = img.info_;
	return *this;
}



bool Eugene::Image::LoadStbFromFile(const std::filesystem::path& path)
{
	int w, h, c;
	auto img = stbi_load(path.string().c_str(), &w, &h, &c, STBI_default);
	if (img == nullptr)
	{
		return false;
	}

	info_.arraySize = 1;
	info_.mipLevels = 1;
	info_.format = Format::R8G8B8A8_UNORM;
	info_.width = w;
	info_.height = h;
	info_.pixelPerBite = c;
	info_.totalSize_ = w * h * c;
	data_.resize(1);
	baseData_.resize(info_.totalSize_);
	data_[0] = { baseData_ };
	std::copy_n(img, data_[0].size(), data_[0].data());
	stbi_image_free(img);
	return true;
}


bool Eugene::Image::LoadStbFromMemory(const std::span<std::byte>& data)
{
	int w, h, c;
	auto img = stbi_load_from_memory(reinterpret_cast<stbi_uc*>(data.data()), static_cast<int>(data.size()), &w, &h, &c, STBI_default);
	if (img == nullptr)
	{
		return false;
	}
	info_.arraySize = 1;
	info_.mipLevels = 1;
	info_.format = Format::R8G8B8A8_UNORM;
	info_.width = w;
	info_.height = h;
	info_.pixelPerBite = c;
	info_.totalSize_ = w * h * c;
	data_.resize(1);
	baseData_.resize(info_.totalSize_);
	data_[0] = { baseData_ };
	std::copy_n(img, data_[0].size(), data_[0].data());
	stbi_image_free(img);
	return true;
}

bool Eugene::Image::LoadDdsFromFile(const std::filesystem::path& path)
{
	std::ifstream file{ path,std::ios::binary };
	if (!file)
	{
		return false;
	}

	int sig;

	// シグネチャ読み込み
	file.read(reinterpret_cast<char*>(&sig), sizeof(sig));
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
	case '2ITA':
		info_.format = Format::BC5_UNORM;
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

	info_.pixelPerBite = std::max(1,h.rgbBitCount / 8);
	data_.resize(info_.arraySize * info_.mipLevels);
	

	// 配列サイズとミップマップの分を読み込む
	for (int j = 0; j < static_cast<int>(info_.arraySize); j++)
	{
		for (int i = 0; i < info_.mipLevels; i++)
		{
			// サイズを計算
			info_.totalSize_ += calcSizeMap.at(info_.format)(
				std::max(1, static_cast<int>(info_.width) >> i),
				std::max(1, static_cast<int>(info_.height) >> i),
				info_.pixelPerBite
				);
		}
	}

	baseData_.resize(info_.totalSize_);
	file.read(reinterpret_cast<char*>(baseData_.data()), info_.totalSize_);

	std::size_t sizeOffset = 0ull;
	for (int j = 0; j < static_cast<int>(info_.arraySize); j++)
	{
		for (int i = 0; i < info_.mipLevels; i++)
		{
			// インデックス
			auto idx{ j * info_.mipLevels + i };
			auto byteSize{ calcSizeMap.at(info_.format)(
				std::max(1, static_cast<int>(info_.width) >> i),
				std::max(1, static_cast<int>(info_.height) >> i),
				info_.pixelPerBite
				) };
			data_[idx] = { &baseData_[sizeOffset],static_cast<std::size_t>(byteSize)};
			sizeOffset += byteSize;
		}
	}

	return true;
}
