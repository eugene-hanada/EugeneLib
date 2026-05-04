#pragma once

#include "Utils.h"
#include "../Math/MathConcepts.h"
#include <cstddef>
#include <span>
#include <cstdint>
#include <type_traits>
#include <string_view>
#include <bit>

namespace Eugene
{
	// Extract bytes MSB->LSB for an unsigned integral value
	template<typename U>
	requires std::unsigned_integral<U>
	constexpr void ExtractBytesMSB(U value, std::uint8_t* out, size_t byteCount) noexcept
	{
		for (size_t i = 0; i < byteCount; ++i)
		{
			out[i] = static_cast<std::uint8_t>((value >> ((byteCount - 1 - i) * 8)) & 0xFFu);
		}
	}


	/// <summary>
	/// 64-bit FNV-1aハッシュ関数
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	template<IntegerByte1 T>
	constexpr const std::uint64_t FNV1aHash64(const T* data, const std::size_t size) noexcept
	{
		constexpr uint64_t FNV_PRIME = 1099511628211ULL;
		constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
		uint64_t hash = FNV_OFFSET_BASIS;
		for (std::size_t i = 0; i < size; i++)
		{
			hash ^= static_cast<const std::uint8_t>(data[i]);
			hash *= FNV_PRIME;
		}
		return hash;
	}

	/// <summary>
	/// FNV1aでハッシュ値を生成する
	/// </summary>
	/// <typeparam name="T"> 2バイトの型 </typeparam>
	/// <param name="data"> 対象データの配列のポインタ </param>
	/// <param name="size"> 数 </param>
	/// <returns></returns>
	template<IntegerByte2 T>
	constexpr const std::uint64_t FNV1aHash64(const T* data, std::size_t size)
	{
		constexpr uint64_t FNV_PRIME = 1099511628211ULL;  // FNV素数 (2^40 + 2^8 + 0xb3)
		constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
		std::uint64_t hash = FNV_OFFSET_BASIS;
		for (std::size_t i = 0; i < size; ++i)
		{
			hash ^= static_cast<const std::uint8_t>(data[i] >> 8 & 0xff);
			hash *= FNV_PRIME;
			hash ^= static_cast<const std::uint8_t>(data[i] & 0xff);
			hash *= FNV_PRIME;
		}
		return hash;
	}

	/// <summary>
	/// FNV1aでハッシュ値を生成する
	/// </summary>
	/// <typeparam name="T"> 4バイトの型 </typeparam>
	/// <param name="data"> 対象データの配列のポインタ </param>
	/// <param name="size"> 数 </param>
	/// <returns></returns>
	template<IntegerByte4 T>
	constexpr const std::uint64_t FNV1aHash64(const T* data, std::size_t size) noexcept
	{
		constexpr uint64_t FNV_PRIME = 1099511628211ULL;  // FNV素数 (2^40 + 2^8 + 0xb3)
		constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
		std::uint64_t hash = FNV_OFFSET_BASIS;
		for (std::size_t i = 0; i < size; ++i)
		{
			hash ^= static_cast<std::uint8_t>(data[i] >> 24 & 0xff);
			hash *= FNV_PRIME;
			hash ^= static_cast<std::uint8_t>(data[i] >> 16 & 0xff);
			hash *= FNV_PRIME;
			hash ^= static_cast<std::uint8_t>(data[i] >> 8 & 0xff);
			hash *= FNV_PRIME;
			hash ^= static_cast<std::uint8_t>(data[i] & 0xff);
			hash *= FNV_PRIME;
		}
		return hash;
	}

	/// <summary>
	/// 32-bit FNV-1aハッシュ関数
	/// </summary>
	/// <param name="bytes"></param>
	/// <returns></returns>
	template<IntegerByte1 T>
	constexpr const std::uint32_t FNV1aHash32(const T* data, std::size_t size) noexcept
	{
		constexpr uint32_t FNV_PRIME = 16777619u;
		constexpr uint32_t FNV_OFFSET_BASIS = 2166136261u;
		uint32_t hash = FNV_OFFSET_BASIS;
		for (std::uint64_t i = 0; i < size; ++i)
		{
			hash ^= static_cast<std::uint8_t>(data[i]);
			hash *= FNV_PRIME;
		}
		return hash;
	}

	/// <summary>
	/// FNV1aでハッシュ値を生成する
	/// </summary>
	/// <typeparam name="T"> 2バイトの型 </typeparam>
	/// <param name="data"> 対象データの配列のポインタ </param>
	/// <param name="size"> 数 </param>
	/// <returns></returns>
	template<IntegerByte2 T>
	constexpr const std::uint32_t FNV1aHash32(const T* data, std::size_t size) noexcept
	{
		constexpr uint32_t FNV_PRIME = 16777619u;
		constexpr uint32_t FNV_OFFSET_BASIS = 2166136261u;
		std::uint32_t hash = FNV_OFFSET_BASIS;
		for (std::size_t i = 0; i < size; ++i)
		{
			hash ^= static_cast<const std::uint8_t>(data[i] >> 8 & 0xff);
			hash *= FNV_PRIME;
			hash ^= static_cast<const std::uint8_t>(data[i] & 0xff);
			hash *= FNV_PRIME;
		}
		return hash;
	}

	/// <summary>
	/// FNV1aでハッシュ値を生成する
	/// </summary>
	/// <typeparam name="T"> 4バイトの型 </typeparam>
	/// <param name="data"> 対象データの配列のポインタ </param>
	/// <param name="size"> 数 </param>
	/// <returns></returns>
	template<IntegerByte4 T>
	constexpr const std::uint32_t FNV1aHash32(const T* data, std::size_t size) noexcept
	{
		constexpr uint32_t FNV_PRIME = 1099511628211ULL;  // FNV素数 (2^40 + 2^8 + 0xb3)
		constexpr uint32_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
		std::uint32_t hash = FNV_OFFSET_BASIS;
		for (size_t i = 0; i < size; ++i)
		{
			hash ^= static_cast<std::uint8_t>(data[i] >> 24 & 0xff);
			hash *= FNV_PRIME;
			hash ^= static_cast<std::uint8_t>(data[i] >> 16 & 0xff);
			hash *= FNV_PRIME;
			hash ^= static_cast<std::uint8_t>(data[i] >> 8 & 0xff);
			hash *= FNV_PRIME;
			hash ^= static_cast<std::uint8_t>(data[i] & 0xff);
			hash *= FNV_PRIME;
		}
		return hash;
	}

	// String convenience overloads
	constexpr const std::uint64_t FNV1aHash64(const std::string_view& sv) noexcept
	{
		return FNV1aHash64(sv.data(), sv.size());
	}

	constexpr std::uint32_t FNV1aHash32(const std::string_view& sv) noexcept
	{
		return FNV1aHash32(sv.data(), sv.size());
	}

	constexpr std::uint64_t FNV1aHash64(const std::u16string_view& sv) noexcept
	{
		return FNV1aHash64(sv.data(), sv.size());
	}

	constexpr std::uint32_t FNV1aHash32(const std::u16string_view& sv) noexcept
	{
		return FNV1aHash32(sv.data(), sv.size());
	}

	constexpr std::uint64_t FNV1aHash64(const std::u32string_view& sv) noexcept
	{
		return FNV1aHash64(sv.data(), sv.size());
	}

	constexpr std::uint32_t FNV1aHash32(const std::u32string_view& sv) noexcept
	{
		return FNV1aHash32(sv.data(), sv.size());
	}

	constexpr std::uint64_t FNV1aHash64(const std::wstring_view& sv) noexcept
	{
		return FNV1aHash64(sv.data(), sv.size());
	}

	constexpr std::uint32_t FNV1aHash32(const std::wstring_view& sv) noexcept
	{
		return FNV1aHash32(sv.data(), sv.size());
	}

} // namespace Eugene

