#include "OggVorbis.h"
#define STB_VORBIS_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_vorbis.c"

#include <vector>

Eugene::OggVorbis::OggVorbis(const std::filesystem::path& path) :
	ptr_{nullptr}
{
	int error = 0;
	stb_vorbis_alloc* alloc = nullptr;
	ptr_ = stb_vorbis_open_filename(path.string().c_str(), &error, alloc);
}

Eugene::OggVorbis::~OggVorbis()
{
	if (ptr_ != nullptr)
	{
		Close();
	}
}

void Eugene::OggVorbis::LoadFormat(void)
{
	auto info = stb_vorbis_get_info(ptr_);
	format_.type = 1;
	format_.channel = info.channels;
	format_.sample = info.sample_rate;
	format_.block = 2;
	format_.byte = format_.sample * format_.block;
	format_.bit = 16;
	format_.size = 16;
}

void Eugene::OggVorbis::LoadData(void)
{
	data_.resize(ptr_->known_loc_for_packet * 2);
	auto info = stb_vorbis_get_info(ptr_);
	auto tmp = stb_vorbis_get_samples_short_interleaved(ptr_, info.channels, reinterpret_cast<std::int16_t*>(data_.data()), ptr_->known_loc_for_packet);
}

void Eugene::OggVorbis::Close(void)
{
	stb_vorbis_close(ptr_);
	ptr_ = nullptr;
}

