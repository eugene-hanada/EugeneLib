#include "SoundStreamFile.h"
#include <fstream>
#include "../../Include/Sound/SoundFile.h"

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
			int id = 0;

			// fmtチャンクを見つける
			while (true)
			{
				file_.read(reinterpret_cast<char*>(&id), 4);
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
				auto now = file_.tellg();
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

	class OggStreamFile :
		public SoundStreamFile
	{
	public:

		OggStreamFile(const std::filesystem::path& path)
		{

		}

		// SoundStreamFile を介して継承されました
		void Read(std::uint8_t* ptr, std::uint64_t size) final
		{
		}

		void SeekStart() final
		{
		}

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

	}

	return std::unique_ptr<SoundStreamFile>();
}
