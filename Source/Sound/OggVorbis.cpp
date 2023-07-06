#include "OggVorbis.h"
#include "../../../Include/Math/Geometry.h"
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
	format_.block = (16u * format_.channel) / 8u;
	format_.byte = format_.sample * format_.block;
	format_.bit = 16;
	format_.size = 16;
}

void Eugene::OggVorbis::LoadData(void)
{
	auto length = stb_vorbis_stream_length_in_samples(ptr_);
	auto channelParLength = length / format_.channel;
	data_.resize(AlignmentedSize( length * 2u * format_.channel, format_.block));
	auto size = 0u;
	for (int i = 0; i < format_.channel; i++)
	{
		size = stb_vorbis_get_samples_short_interleaved(ptr_, format_.channel, reinterpret_cast<std::int16_t*>(data_.data() + AlignmentedSize(length * 2 * i,format_.block)) , length);
	}

}

void Eugene::OggVorbis::Close(void)
{
	stb_vorbis_close(ptr_);
	ptr_ = nullptr;
}

