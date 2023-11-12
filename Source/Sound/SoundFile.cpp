#include "../../Include/Sound/SoundFile.h"
#include "../../Include/Math/Geometry.h"
#include <fstream>
#include "../../Include/Common/EugeneLibException.h"
#define STB_VORBIS_IMPLEMENTATION
#include "../../Include/ThirdParty/stb/stb_vorbis.c"

//#include "Wave.h"
//#include "OggVorbis.h"

//Eugene::SoundFile* Eugene::OpenSoundFile(const std::filesystem::path& path)
//{
//    auto ext = path.extension();
//    //if (ext == ".ogg")
//    //{
//    //    return new OggVorbis{path};
//    //}
//    //else
//    //{
//    //    return new Wave{path};
//    //}
//    return nullptr;
//
//}

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
    auto ext = path.extension();
    if (ext == ".ogg")
    {
        int error = 0;
        stb_vorbis_alloc* alloc = nullptr;
        auto ptr = stb_vorbis_open_filename(path.string().c_str(), &error, alloc);
        if (ptr == nullptr)
        {
            throw CreateErrorException("ファイルが開けませんでした");
        }
        auto info = stb_vorbis_get_info(ptr);
        format_.type = 1;
        format_.channel = info.channels;
        format_.sample = info.sample_rate;
        format_.block = (16u * format_.channel) / 8u;
        format_.byte = format_.sample * format_.block;
        format_.bit = 16;
        format_.size = 16;

        auto length = stb_vorbis_stream_length_in_samples(ptr);
        auto channelParLength = length / format_.channel;
        data_.resize(AlignmentedSize(length * 2u * format_.channel, format_.block));
        auto size = 0u;
        for (int i = 0; i < format_.channel; i++)
        {
            size = stb_vorbis_get_samples_short_interleaved(ptr, format_.channel, reinterpret_cast<std::int16_t*>(data_.data() + AlignmentedSize(length * 2 * i, format_.block)), length);
        }
        stb_vorbis_close(ptr);
    }
    else
    {
        auto file = std::ifstream{ path , std::ios::binary };
        if (!file)
        {
            throw CreateErrorException("ファイルが開けませんでした");
        }

        RIFF riff;
        file.read(reinterpret_cast<char*>(&riff), sizeof(riff));
        int id = 0;
        while (true)
        {
            file.read(reinterpret_cast<char*>(&id), sizeof(id));
            if (file.eof())
            {
                return;
            }

            if (id == fmt)
            {
                file.read(reinterpret_cast<char*>(&format_), sizeof(format_));
                if (format_.type == 1u)
                {
                    auto now = file.tellg();
                    now -= 2ull;
                    file.seekg(now);
                    format_.ex = 0u;
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
    }

   
}

Eugene::SoundFile::SoundFile(SoundFile&& soundFile) :
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
