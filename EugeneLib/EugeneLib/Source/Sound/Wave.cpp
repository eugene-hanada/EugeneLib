#include "../../Include/Sound/Wave.h"
#include <fstream>
#include "../../Include/Common/EugeneLibException.h"

#include "../../Include/Common/Debug.h"

EugeneLib::Wave::Wave(const std::filesystem::path& path) :
	fmt_{}
{
	std::ifstream file{ path , std::ios::binary };

	if (!file)
	{
		throw EugeneLibException("�t�@�C�����J���܂���ł���");
	}

	RIFF riff;
	file.read(reinterpret_cast<char*>(&riff), sizeof(riff));

	std::string id;
	id.resize(4);
	std::string fmtStr{ 'f','m','t',' ' };
	std::string dataStr{ 'd','a','t','a' };
	while (true)
	{
		file.read(id.data(), 4);
		if (file.eof())
		{
			return;
		}

		if (id == fmtStr)
		{
			LoadFmt(file);
		}
		else if (id == dataStr)
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

void EugeneLib::Wave::LoadFmt(std::ifstream& file)
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

void EugeneLib::Wave::LoadData(std::ifstream& file)
{
	std::uint32_t size{ 0 };
	file.read(reinterpret_cast<char*>(&size), sizeof(size));
	data_.resize(size);
	file.read(reinterpret_cast<char*>(data_.data()), size);
}
