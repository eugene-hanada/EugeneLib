#include "Wave.h"
#include <fstream>
#include "../../Include/Common/EugeneLibException.h"

#include "../../Include/Common/Debug.h"
#include "Wave.h"

Eugene::Wave::Wave(const std::filesystem::path& path) :
	SoundFile{}
{
	file_ = std::ifstream{ path , std::ios::binary };
	if (!file_)
	{
		throw CreateErrorException("ファイルが開けませんでした");
	}

	isFormatLoaded_ = false;

	RIFF riff;
	file_.read(reinterpret_cast<char*>(&riff), sizeof(riff));

	//int id = 0;
	//while (true)
	//{
	//	file_.read(reinterpret_cast<char*>(&id), sizeof(id));
	//	if (file_.eof())
	//	{
	//		return;
	//	}

	//	if (id == Wave::fmt)
	//	{
	//		LoadFmt(file_);
	//	}
	//	else if (id == Wave::data)
	//	{
	//		LoadData(file_);
	//	}
	//	else
	//	{
	//		std::uint32_t size{ 0 };
	//		file_.read(reinterpret_cast<char*>(&size), sizeof(size));
	//		file_.ignore(size);
	//	}
	//}

}

Eugene::Wave::~Wave()
{

}

void Eugene::Wave::LoadFormat(void)
{
	if (file_.is_open() && !isFormatLoaded_)
	{
		int id = 0;
		while (true)
		{
			file_.read(reinterpret_cast<char*>(&id), sizeof(id));
			if (file_.eof())
			{
				break;
			}

			if (id == Wave::fmt)
			{
				LoadFmt(file_);
				file_.seekg(0);
				file_.ignore(sizeof(RIFF));

				return;
			}
			else
			{
				std::uint32_t size{ 0 };
				file_.read(reinterpret_cast<char*>(&size), sizeof(size));
				file_.ignore(size);
			}
		}
	}
}

void Eugene::Wave::LoadData(void)
{
	if (file_.is_open() && data_.size() <= 0ull)
	{
		int id = 0;
		while (true)
		{
			file_.read(reinterpret_cast<char*>(&id), sizeof(id));
			if (file_.eof())
			{
				break;
			}

			if (id == Wave::data)
			{
				LoadData(file_);
				file_.seekg(0);
				file_.ignore(sizeof(RIFF));
				return;
			}
			else
			{
				std::uint32_t size{ 0 };
				file_.read(reinterpret_cast<char*>(&size), sizeof(size));
				file_.ignore(size);
			}
		}
	}
}

void Eugene::Wave::Close(void)
{
	file_.close();
}

void Eugene::Wave::LoadFmt(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&format_), sizeof(format_));
	if (format_.type == 1u)
	{
		auto now = file.tellg();
		now -= 2ull;
		file.seekg(now);
		format_.ex = 0u;
		return;
	}
	file.read(reinterpret_cast<char*>(&ex_), sizeof(ex_));
}

void Eugene::Wave::LoadData(std::ifstream& file)
{
	std::uint32_t size{ 0 };
	file.read(reinterpret_cast<char*>(&size), sizeof(size));
	data_.resize(size);
	file.read(reinterpret_cast<char*>(data_.data()), size);
}
