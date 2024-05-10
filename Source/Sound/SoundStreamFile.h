#pragma once
#include <filesystem>
#include <memory>
#include "../../Include/Sound/SoundCommon.h"

namespace Eugene
{
	class SoundStreamFile
	{
	public:
		virtual ~SoundStreamFile();
		const SoundFormat& GetFormat() const;
		const SoundFormatEx& GetFormatEx() const;
		const std::uint64_t GetDataSize() const;
		const std::uint64_t GetReadSize() const;
		virtual void Read(std::uint8_t* ptr, std::uint64_t size) = 0;
		virtual void SeekStart() = 0;
	protected:
		SoundStreamFile();

		SoundFormat format_;

		SoundFormatEx formatEx_;

		std::uint64_t dataSize_;
		std::uint64_t readSize_;

	};

	std::unique_ptr<SoundStreamFile> CreateSoundStreamFile(const std::filesystem::path& path);
}
