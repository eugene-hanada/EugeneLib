#include "../../Include/Sound/SoundFile.h"
#include "../../Include/Utils/Utils.h"
#include <fstream>
#include "../../Include/Utils/EugeneLibException.h"
#define STB_VORBIS_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_vorbis.c"


const std::uint8_t* Eugene::SoundFile::GetDataPtr(void) const noexcept
{
	if (data_.size() <= 0ull)
	{
		return nullptr;
	}
	return data_.data();
}

const std::uint64_t Eugene::SoundFile::GetDataSize(void) const noexcept
{
	return data_.size();
}

const Eugene::SoundFormat& Eugene::SoundFile::GetFormat(void) const noexcept
{
	return format_;
}

const Eugene::SoundFormatEx& Eugene::SoundFile::GetFormatExt(void) const noexcept
{
	return ex_;
}

Eugene::SoundFile::SoundFile(const std::filesystem::path& path)
{
	auto ext{ path.extension() };
	if (ext == ".ogg")
	{
		if (!LoadOggVorbis(path))
		{
			throw CreateErrorException("OggVorbissファイルが開けませんでした");
		}
	}
	else
	{
		if (!LoadWave(path))
		{
			throw CreateErrorException("Waveファイルが開けませんでした");
		}
	}
}



Eugene::SoundFile::SoundFile(SoundFile&& soundFile)noexcept :
	ex_{soundFile.ex_}, format_{soundFile.format_}
{
	data_ = std::move(soundFile.data_);
}

Eugene::SoundFile& Eugene::SoundFile::operator=(SoundFile&& soundFile)
{
	data_ = std::move(soundFile.data_);
   format_ = soundFile.format_;
   ex_ = soundFile.ex_;
	return *this;
}

bool Eugene::SoundFile::LoadWave(const std::filesystem::path& path)
{
	auto file = std::ifstream{ path , std::ios::binary };
	if (!file)
	{
		return false;
	}

	RIFF riff;
	file.read(reinterpret_cast<char*>(&riff), sizeof(riff));
	int id = 0;
	while (true)
	{
		file.read(reinterpret_cast<char*>(&id), sizeof(id));
		if (file.eof())
		{
			return true;
		}

		if (id == fmt)
		{
			file.ignore(4);
			file.read(reinterpret_cast<char*>(&format_), sizeof(format_));
			if (format_.type == 1u)
			{
				auto now = file.tellg();
				now -= 2ull;
				file.seekg(now);
				//format_.ex = 0u;
				continue;
			}
			file.read(reinterpret_cast<char*>(&ex_), sizeof(ex_));
			continue;
		}
		else if (id == data)
		{
			std::uint32_t size{ 0 };
			file.read(reinterpret_cast<char*>(&size), sizeof(size));
			data_.resize(size);
			file.read(reinterpret_cast<char*>(data_.data()), size);
			continue;
		}
		else
		{
			std::uint32_t size{ 0 };
			file.read(reinterpret_cast<char*>(&size), sizeof(size));
			file.ignore(size);
			continue;
		}
	}

	return true;
}

bool Eugene::SoundFile::LoadOggVorbis(const std::filesystem::path& path)
{
	int error = 0;
	stb_vorbis_alloc* alloc{ nullptr };
	auto ptr{ stb_vorbis_open_filename(path.string().c_str(), &error, alloc) };
	if (ptr == nullptr)
	{
		return false;
	}
	auto info = stb_vorbis_get_info(ptr);
	format_.type = 1;
	format_.channel = info.channels ;
	format_.sample = info.sample_rate;
	format_.block = (16u * format_.channel) / 8u;
	format_.byte = format_.sample * format_.block;
	format_.bit = 16;
	auto length{ stb_vorbis_stream_length_in_samples(ptr) };
	auto totalSamples {length * info.channels };
	data_.resize(AlignmentedSize(sizeof(std::int16_t) * totalSamples,format_.block));
	stb_vorbis_get_samples_short_interleaved(ptr, info.channels, reinterpret_cast<std::int16_t*>(data_.data()), totalSamples);
	stb_vorbis_close(ptr);
	return true;
}
