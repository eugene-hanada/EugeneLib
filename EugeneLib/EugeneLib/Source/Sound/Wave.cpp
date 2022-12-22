#include "../../Include/Sound/Wave.h"
#include <fstream>
#include "../../Include/Common/EugeneLibException.h"

EugeneLib::Wave::Wave(const std::filesystem::path& path) :
	fmt_{}
{
	std::ifstream file{ path , std::ios::binary };

	if (!file)
	{
		throw EugeneLibException("ファイルが開けませんでした");
	}

	RIFF riff;
	file.read(reinterpret_cast<char*>(&riff), sizeof(riff));

	std::string id;
	id.resize(4);
	std::string fmtStr{ 'f','m','t',' ' };

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
		else
		{
			LoadData(file);
		}
	}

}

void EugeneLib::Wave::LoadFmt(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&fmt_), sizeof(fmt_));
}

void EugeneLib::Wave::LoadData(std::ifstream& file)
{
	std::uint32_t size{ 0 };
	file.read(reinterpret_cast<char*>(&size), sizeof(size));
	data_.resize(size);
	file.read(reinterpret_cast<char*>(data_.data()), size);
}
