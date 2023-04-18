#include "../../Include/Sound/SoundFile.h"

#include "Wave.h"
#include "OggVorbis.h"

Eugene::SoundFile* Eugene::OpenSoundFile(const std::filesystem::path& path)
{
    auto ext = path.extension();
    if (ext == ".ogg")
    {
        return nullptr;
    }
    else
    {
        return new Wave{path};
    }

}

const std::uint8_t* Eugene::SoundFile::GetDataPtr(void) const
{
    if (data_.size() <= 0ull)
    {
        return nullptr;
    }
    return data_.data();
}

const std::uint64_t Eugene::SoundFile::GetDataSize(void) const
{
    return data_.size() * sizeof(std::uint8_t);
}

const Eugene::SoundFormat& Eugene::SoundFile::GetFormat(void) const
{
    return format_;
}

const Eugene::SoundFormatEx& Eugene::SoundFile::GetFormatExt(void) const
{
    return ex_;
}

Eugene::SoundFile::SoundFile()
{
}

Eugene::SoundFile::~SoundFile()
{
}
