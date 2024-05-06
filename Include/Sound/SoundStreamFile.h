#pragma once
#include <filesystem>
#include "SoundCommon.h"

namespace Eugene
{
	class SoundStreamFile
	{
	public:
		SoundStreamFile(const std::filesystem::path& path);
	};
}
