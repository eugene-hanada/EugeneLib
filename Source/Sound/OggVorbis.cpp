#include "../../Include/Sound/OggVorbis.h"
#define STB_VORBIS_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_vorbis.c"

#include <vector>

Eugene::OggVorbis::OggVorbis(const std::filesystem::path& path)
{
	int error = 0;
	stb_vorbis_alloc* alloc = nullptr;

	stb_vorbis* ptr = stb_vorbis_open_filename(path.string().c_str(), &error, alloc);
	auto info = stb_vorbis_get_info(ptr);
	
	data_.resize(ptr->known_loc_for_packet);
	
	auto tmp = stb_vorbis_get_samples_short_interleaved(ptr,info.channels, data_.data(), data_.size());
	format_.type = 1;
	format_.channel = info.channels;
	format_.sample = info.sample_rate;
	format_.block = 2;
	format_.byte = format_.sample * format_.block;
	format_.bit = 16;
	format_.size = 16;
	
	

	stb_vorbis_close(ptr);
}

const std::uint8_t* Eugene::OggVorbis::GetDataPtr(void) const
{
	return reinterpret_cast<const std::uint8_t*>(data_.data());
}

const std::uint64_t Eugene::OggVorbis::GetDataSize(void) const
{
	return data_.size() * sizeof(data_[0]);
}

const Eugene::SoundFormat& Eugene::OggVorbis::GetFormat(void) const
{
	return format_;
}
