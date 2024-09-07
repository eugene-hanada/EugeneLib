#include "../../../Include/Sound/SoundStreamFile.h"
#include <fstream>
#include "../../Include/Sound/SoundFile.h"
#define STB_VORBIS_HEADER_ONLY
#include "../../Include/ThirdParty/stb/stb_vorbis.c"

namespace Eugene
{
	/// <summary>
	/// Waveファイルを読み込むファイルストリームクラス
	/// </summary>
	class WaveStreamFile :
		public SoundStreamFile
	{
	public:

		WaveStreamFile(const std::filesystem::path& path)
		{
			file_ = std::ifstream{ path, std::ios::binary };
			SoundFile::RIFF riff;
			file_.read(reinterpret_cast<char*>(&riff), sizeof(riff));
			std::int32_t id{ 0 };

			// fmtチャンクを見つける
			while (true)
			{
				file_.read(reinterpret_cast<char*>(&id), sizeof(id));
				if (SoundFile::fmt == id)
				{
					break;
				}

			}

			file_.ignore(4);
			file_.read(reinterpret_cast<char*>(&format_), sizeof(format_));
			if (format_.type != 1)
			{
				file_.read(reinterpret_cast<char*>(&formatEx_), sizeof(formatEx_));
			}
			else
			{
				auto now{ file_.tellg() };
				now -= 2ull;
				file_.seekg(now);
			}

			// データチャンクを見つける
			while (true)
			{
				file_.read(reinterpret_cast<char*>(&id), 4);
				if (SoundFile::data == id)
				{
					break;
				}

			}

			file_.read(reinterpret_cast<char*>(&dataSize_), sizeof(dataSize_));

			// データ開始位置をセット
			startPos_ = file_.tellg();
		}

		// SoundStreamFile を介して継承されました
		void Read(std::uint8_t* ptr, std::uint64_t size) final
		{
			file_.read(reinterpret_cast<char*>(ptr), size);
			readSize_ += size;
		}

		void SeekStart() final
		{
			file_.clear();
			file_.seekg(startPos_);
			readSize_ = 0;
		}

	private:

		/// <summary>
		/// ファイルストリーム
		/// </summary>
		std::ifstream file_;

		/// <summary>
		/// サウンドデータの開始位置
		/// </summary>
		std::streampos startPos_;
	};

	/// <summary>
	/// OggVorbisファイルのファイルストリーム
	/// </summary>
	class OggVorbisStreamFile :
		public SoundStreamFile
	{
	public:

		OggVorbisStreamFile(const std::filesystem::path& path) :
			allocPtr_{nullptr}
		{
			int error{ 0 };
			vorbisPtr_ = stb_vorbis_open_filename(path.string().c_str(), &error, allocPtr_);

			auto info{ stb_vorbis_get_info(vorbisPtr_) };
			format_.type = 1;
			format_.channel = info.channels;
			format_.sample = info.sample_rate;
			format_.block = (16u * format_.channel) / 8u;
			format_.byte = format_.sample * format_.block;
			format_.bit = 16;
			dataSize_ = stb_vorbis_stream_length_in_samples(vorbisPtr_) * format_.channel * format_.block;
		}

		~OggVorbisStreamFile()
		{
			stb_vorbis_close(vorbisPtr_);
		}

		// SoundStreamFile を介して継承されました
		void Read(std::uint8_t* ptr, std::uint64_t size) final
		{
			auto sampleNum{ size / sizeof(std::int16_t)};
			stb_vorbis_get_samples_short_interleaved(vorbisPtr_, format_.channel, reinterpret_cast<std::int16_t*>(ptr), sampleNum);
			readSize_ += size;
		}

		void SeekStart() final
		{
			stb_vorbis_seek_start(vorbisPtr_);
			readSize_ = 0ull;
		}

		/// <summary>
		/// stb_vorbisのallocポインタ
		/// </summary>
		stb_vorbis_alloc* allocPtr_;

		/// <summary>
		/// stb_vorbisのポインタ
		/// </summary>
		stb_vorbis* vorbisPtr_;

	};
}

Eugene::SoundStreamFile::~SoundStreamFile()
{
}

const Eugene::SoundFormat& Eugene::SoundStreamFile::GetFormat() const
{
	return format_;
}

const Eugene::SoundFormatEx& Eugene::SoundStreamFile::GetFormatEx() const
{
	return formatEx_;
}

const std::uint64_t Eugene::SoundStreamFile::GetDataSize() const
{
	return dataSize_;
}

const std::uint64_t Eugene::SoundStreamFile::GetReadSize() const
{
	return readSize_;
}

Eugene::SoundStreamFile::SoundStreamFile() :
	dataSize_{0},formatEx_{},readSize_{0}
{
}

std::unique_ptr<Eugene::SoundStreamFile> Eugene::CreateSoundStreamFile(const std::filesystem::path& path)
{
	const auto& ext = path.extension();

	if (ext == ".wav")
	{
		return std::make_unique<WaveStreamFile>(path);
	}
	else if (ext == ".ogg")
	{
		return std::make_unique<OggVorbisStreamFile>(path);
	}

	return std::unique_ptr<SoundStreamFile>();
}
