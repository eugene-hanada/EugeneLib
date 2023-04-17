#pragma once
#include <filesystem>
#include <vector>
#include "SoundCommon.h"

namespace Eugene
{
	class OggVorbis
	{
	public:
		OggVorbis(const std::filesystem::path& path);
		SoundFormat format_;
		const std::uint8_t* GetDataPtr(void) const;
		const std::uint64_t GetDataSize(void) const;
		std::vector<std::int16_t> data_;
	};
}
