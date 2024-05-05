#pragma once
#include <bit>

namespace Eugene
{
	/// <summary>
	/// アライメントされた大きさを返す
	/// </summary>
	/// <param name="size"> サイズ </param>
	/// <param name="alignment"> アライメントサイズ </param>
	/// <returns> アライメントされたサイズ </returns>
	constexpr size_t AlignmentedSize(const size_t& size, const size_t& alignment) noexcept
	{
		if ((alignment & ~0xfffffffffffffffd) == 0ull)
		{
			return (size + alignment - 1ull) & ~(alignment - 1ull);
		}
		return (size + alignment - 1ull) - (size % alignment - 1ull);
	}

	/// <summary>
	/// バイト列を並び帰る
	/// </summary>
	/// <param name="data"> バイト列のポインタ </param>
	/// <param name="size"> バイト数 </param>
	constexpr void BytesSwap(std::uint8_t* data, std::uint64_t size)
	{
		for (std::uint64_t i = 0ull; i < size / 2ull; ++i)
		{
			std::swap(data[i], data[size - i - 1ull]);
		}
	}
}
