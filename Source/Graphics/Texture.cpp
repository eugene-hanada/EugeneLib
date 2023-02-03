#include "../../Include/Graphics/Texture.h"
#include "../../Include/Common/EugeneLibException.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_image.h"


Eugene::Texture::Texture(const std::filesystem::path& path)
{
	int w, h, c;
	auto img = stbi_load(path.string().c_str(), &w, &h, &c, STBI_default);

	if (img == nullptr)
	{
		throw EugeneLibException("ファイル読み込みエラー");
	}

	data_.resize(w * h * c);
	std::copy_n(img, data_.size(), data_.data());
	info_.arraySize = 1;
	info_.format = Format::R8G8B8A8_UNORM;
	info_.width = w;
	info_.height = h;
	stbi_image_free(img);
}

const Eugene::TextureInfo& Eugene::Texture::GetInfo(void) const&
{
	return info_;
}


std::uint8_t* Eugene::Texture::GetData(void)&
{
	return data_.data();
}
