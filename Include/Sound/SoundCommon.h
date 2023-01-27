#pragma once
#include <cstdint>

namespace Eugene
{
#pragma pack(1)
	struct SoundFormat
	{
		std::uint32_t size = 0u;
		std::uint16_t type = 0u;
		std::uint16_t channel = 0u;
		std::uint32_t sample = 0u;
		std::uint32_t byte = 0u;
		std::uint16_t block = 0u;
		std::uint16_t bit = 0u;
		std::uint16_t ex = 0u;
	};

	struct SoundFormatEx
	{
		std::uint16_t reserved;
		std::uint32_t channelMask;
		std::uint32_t d1;
		std::uint16_t d2;
		std::uint16_t  d3;
		std::uint8_t  d4[8];
	};
#pragma pack()

}
