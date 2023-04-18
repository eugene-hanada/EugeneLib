#pragma once
#include <filesystem>
#include <vector>
#include "../../Include/Sound/SoundFile.h"

struct stb_vorbis;

namespace Eugene
{
	class OggVorbis :
		public SoundFile
	{
	public:
		OggVorbis(const std::filesystem::path& path);
		~OggVorbis();
	private:
		void LoadFormat(void) final;

		void LoadData(void)final;

		void Close(void)final;

		stb_vorbis* ptr_;
	};
}
