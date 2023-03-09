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


std::uint8_t* Eugene::Image::GetData(std::uint16_t mipMapLevel)
{
	return data_.at(mipMapLevel).data();
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
	
	file.read(reinterpret_cast<char*>(&sig), 4);
	if (sig != ddsSig)
	{
		return false;
	}

	DdsHeader h;
	file.read(reinterpret_cast<char*>(&h), sizeof(h));

	info_.arraySize = 0;
	info_.mipLevels = h.mipMapCount;
	info_.height = h.height;
	info_.width = h.width;

	if (h.fourCC == '01XD')
	{
		DdsExtensionHeader ext;
		file.read(reinterpret_cast<char*>(&ext), sizeof(ext));
		return true;
	}
	h.fourCC = 0;
	
	auto pixelPerPite = h.rgbBitCount / 8;
	data_.resize(h.mipMapCount);
	info_.format = Format::R8G8B8A8_UNORM;

	
	for (int i = 0; i < h.mipMapCount; i++)
	{
		auto divNum = std::max(static_cast<int>(std::pow(2,i)), 1);
		auto size = colcMap.at(h.fourCC)(h.width / divNum,h.height / divNum, pixelPerPite);
		data_[i].resize(size);
		file.read(reinterpret_cast<char*>(data_[i].data()), size);
	}

	return true;
}
