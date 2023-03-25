#include "../../Include/Sound/Wave.h"
#include <fstream>
#include "../../Include/Common/EugeneLibException.h"

#include "../../Include/Common/Debug.h"

Eugene::Wave::Wave(const std::filesystem::path& path) :
	fmt_{}, exData_{}
{
	std::ifstream file{ path , std::ios::binary };
	if (!file)
	{
		throw EugeneLibException("ファイルが開けませんでした");
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

		if (id == Wave::fmt)
		{
			LoadFmt(file);
		}
		else if (id == Wave::data)
		{
			LoadData(file);
		}
		else
		{
			std::uint32_t size{ 0 };
			file.read(reinterpret_cast<char*>(&size), sizeof(size));
			file.ignore(size);
		}
	}

}

void Eugene::Wave::LoadFmt(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&fmt_), sizeof(fmt_));
	if (fmt_.type == 1u)
	{
		auto now = file.tellg();
		now -= 2ull;
		file.seekg(now);
		fmt_.ex = 0u;
		return;
	}
	file.read(reinterpret_cast<char*>(&exData_), sizeof(exData_));
}

void Eugene::Wave::LoadData(std::ifstream& file)
{
	std::uint32_t size{ 0 };
	file.read(reinterpret_cast<char*>(&size), sizeof(size));
	data_.resize(size);
	file.read(reinterpret_cast<char*>(data_.data()), size);
}
