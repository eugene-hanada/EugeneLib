#pragma once
#include <cstdint>

namespace EugeneLib
{
	struct SoundFormat
	{
		std::uint32_t size = 0u;
		std::uint16_t type = 0u;
		std::uint16_t channel = 0u;
		std::uint32_t sample = 0u;
		std::uint32_t byte = 0u;
		std::uint16_t block = 0u;
		std::uint16_t bit = 0u;
	};
}
