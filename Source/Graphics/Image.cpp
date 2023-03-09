#include "../../Include/Graphics/Image.h"
#include "../../Include/Common/EugeneLibException.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_image.h"

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

constexpr int ddsSig = 0x20534444;

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


std::uint8_t* Eugene::Image::GetData(void)
{
	return data_.data();
}

bool Eugene::Image::LoadStb(const std::filesystem::path& path)
{
	int w, h, c;
	auto img = stbi_load(path.string().c_str(), &w, &h, &c, STBI_default);

	if (img == nullptr)
	{
		return false;
	}

	data_.resize(w * h * c);
	std::copy_n(img, data_.size(), data_.data());
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
	return true;
}
