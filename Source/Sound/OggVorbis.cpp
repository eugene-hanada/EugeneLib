#include "../../Include/Sound/OggVorbis.h"
#define STB_VORBIS_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_vorbis.c"



Eugene::OggVorbis::OggVorbis(const std::filesystem::path& path)
{
	int error = 0;
	stb_vorbis* ptr = stb_vorbis_open_filename(path.string().c_str(), &error, nullptr);

	stb_vorbis_close(ptr);
}
