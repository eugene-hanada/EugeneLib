#include "../../Include/Graphics/Image.h"
#include "../../Include/Common/EugeneLibException.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_image.h"

#include "DdsLoad.h"

const Eugene::Image::LoadFuncMap Eugene::Image::loadFuncMap_{
	{std::hash<std::string>()(".png"),{&Image::LoadStbInfo,&Image::LoadStbData}},
	{std::hash<std::string>()(".jpeg"),{&Image::LoadStbInfo,&Image::LoadStbData}},
	{std::hash<std::string>()(".tga"),{&Image::LoadStbInfo,&Image::LoadStbData}},
	{std::hash<std::string>()(".bmp"),{&Image::LoadStbInfo,&Image::LoadStbData}},
	{std::hash<std::string>()(".psd"),{&Image::LoadStbInfo,&Image::LoadStbData}},
	{std::hash<std::string>()(".gif"),{&Image::LoadStbInfo,&Image::LoadStbData}},
	{std::hash<std::string>()(".hdr"),{&Image::LoadStbInfo,&Image::LoadStbData}},
	{std::hash<std::string>()(".pic"),{&Image::LoadStbInfo,&Image::LoadStbData}},
	{std::hash<std::string>()(".pnm"),{&Image::LoadStbInfo,&Image::LoadStbData}},
	{std::hash<std::string>()(".dds"),{&Image::LoadDdsInfo,&Image::LoadDdsData}}
};

constexpr int ddsSig = std::endian::native == std::endian::little ? 542327876 : 7678324205;

Eugene::Image::Image(const std::filesystem::path& path) :
	info_{}, br_{path}, isInfoLoaded_{false}
{
	ext_ = std::hash<std::string>()(path.extension().string());
}

const Eugene::TextureInfo& Eugene::Image::GetInfo(void) const&
{
	return info_;
}


std::uint8_t* Eugene::Image::GetData(std::uint32_t arrayIndex, std::uint16_t mipMapLevel)
{
	return data_.at((arrayIndex * info_.mipLevels) + mipMapLevel).data();
}

void Eugene::Image::LoadData(void)
{
	(this->*loadFuncMap_.at(ext_).second)(br_);
}

void Eugene::Image::LoadInfo(void)
{
	(this->*loadFuncMap_.at(ext_).first)(br_);
}

void Eugene::Image::Load(void)
{
	LoadInfo();
	LoadData();
}


Eugene::Image::Image(Image&& img) :
	info_{img.info_}, br_{ img.br_ }, isInfoLoaded_{ img.isInfoLoaded_ }
{
	data_ = std::move(img.data_);
}

Eugene::Image& Eugene::Image::operator=(Image&& img)
{
	data_ = std::move(img.data_);
	info_ = img.info_;
	return *this;
}

bool Eugene::Image::LoadStbInfo(BinaryReader& br)
{
	if (isInfoLoaded_)
	{
		return false;
	}
	int w, h, c;
	stbi_info_from_file(br.GetFilePtr(), &w, &h, &c);
	info_.arraySize = 1;
	info_.mipLevels = 1;
	info_.format = Format::R8G8B8A8_UNORM;
	info_.width = w;
	info_.height = h;
	isInfoLoaded_ = true;

	if (data_.size() > 0ull)
	{
		// すでにデータが読み込まれているときバイナリリーダーを閉じる
		br.Close();
	}

	return true;
}

bool Eugene::Image::LoadStbData(BinaryReader& br)
{
	if (!isInfoLoaded_)
	{
		return false;
	}
	int w, h, c;
	auto img = stbi_load_from_file(br.GetFilePtr(), &w, &h, &c, STBI_default);
	data_.resize(1);
	data_[0].resize(w * h * c);
	std::copy_n(img, data_[0].size(), data_[0].data());
	stbi_image_free(img);

	if (isInfoLoaded_)
	{
		br_.Close();
	}

	return true;
}

bool Eugene::Image::LoadDdsInfo(BinaryReader& br)
{
	int sig;

	// シグネチャ読み込み
	br.Read(static_cast<void*>(&sig), 4);
	if (sig != ddsSig)
	{
		// シグネチャがあってなかったら読み込みをやめる
		return false;
	}

	// ヘッダー読み込み
	DdsHeader h;
	br.Read(static_cast<void*>(&h), sizeof(h));

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
		LoadDdsExtension(br, info_);
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

	info_.pixelPerBite = h.rgbBitCount / 8;
	isInfoLoaded_ = true;
	return false;
}

bool Eugene::Image::LoadDdsData(BinaryReader& br)
{
	if (!isInfoLoaded_)
	{
		return false;
	}

	data_.resize(info_.arraySize * info_.mipLevels);

	// 配列サイズとミップマップの分を読み込む
	for (int j = 0; j < info_.arraySize; j++)
	{
		for (int i = 0; i < info_.mipLevels; i++)
		{
			// インデックス
			auto idx = j * info_.mipLevels + i;

			// サイズを計算
			auto size = colcMap.at(info_.format)(
				std::max(1, static_cast<int>(info_.width) >> i),
				std::max(1, static_cast<int>(info_.height) >> i),
				info_.pixelPerBite
				);

			// リサイズして読み込む
			data_[idx].resize(size);
			br.Read(static_cast<void*>(data_[idx].data()), size);
		}
	}
	br.Close();
	return true;
}

Eugene::Image::BinaryReader::BinaryReader(const std::filesystem::path& path)
{
#if _WIN64
	fopen_s(&file_,path.string().c_str(), "rb");
#endif
	if (file_ == nullptr)
	{
		throw EugeneLibException{ "ファイルオープンに失敗" };
	}
}

Eugene::Image::BinaryReader::~BinaryReader()
{
	if (file_ != nullptr)
	{
		Close();
	}
}

void Eugene::Image::BinaryReader::Read(void* ptr, std::uint64_t size)
{
	std::fread(ptr, size, 1, file_);
}

bool Eugene::Image::BinaryReader::IsOpen(void) const
{
	return file_ != nullptr;
}

void Eugene::Image::BinaryReader::Close(void)
{
	std::fclose(file_);
	file_ = nullptr;
}

 FILE* Eugene::Image::BinaryReader::GetFilePtr(void) 
{
	return file_;
}
