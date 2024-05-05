#pragma once
#include "Utils.h"
#include "../Math/MathConcepts.h"

namespace Eugene
{
	/// <summary>
	/// FNV1a�Ńn�b�V���l�𐶐�����
	/// </summary>
	/// <typeparam name="T"> 1�o�C�g�̌^ </typeparam>
	/// <param name="data"> �Ώۃf�[�^�̔z��̃|�C���^ </param>
	/// <param name="size"> �� </param>
	/// <returns></returns>
	template<IntegerByte1 T>
	constexpr std::uint64_t FNV1aHash64(const T* data, std::uint64_t size) {
		constexpr uint64_t FNV_PRIME = 1099511628211ULL;  // FNV�f�� (2^40 + 2^8 + 0xb3)
		constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
		std::uint64_t hash = FNV_OFFSET_BASIS;

		if constexpr (std::endian::native == std::endian::big)
		{
			BytesSwap(data, size);
		}

		for (size_t i = 0; i < size; ++i) {
			hash ^= static_cast<std::uint8_t>(data[i]);
			hash *= FNV_PRIME;
		}

		return hash;
	}

	/// <summary>
	/// FNV1a�Ńn�b�V���l�𐶐�����
	/// </summary>
	/// <typeparam name="T"> 2�o�C�g�̌^ </typeparam>
	/// <param name="data"> �Ώۃf�[�^�̔z��̃|�C���^ </param>
	/// <param name="size"> �� </param>
	/// <returns></returns>
	template<IntegerByte2 T>
	constexpr std::uint64_t FNV1aHash64(const T* data, std::uint64_t size) {
		constexpr uint64_t FNV_PRIME = 1099511628211ULL;  // FNV�f�� (2^40 + 2^8 + 0xb3)
		constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;

		std::uint64_t hash = FNV_OFFSET_BASIS;

		if constexpr (std::endian::native == std::endian::big)
		{
			BytesSwap(data, size * sizeof(T));
		}

		for (size_t i = 0; i < size; ++i) {
			hash ^= static_cast<std::uint8_t>(data[i] >> 8 & 0xff);
			hash *= FNV_PRIME;
			hash ^= static_cast<std::uint8_t>(data[i] & 0xff);
			hash *= FNV_PRIME;
		}

		return hash;
	}

	/// <summary>
	/// FNV1a�Ńn�b�V���l�𐶐�����
	/// </summary>
	/// <typeparam name="T"> 4�o�C�g�̌^ </typeparam>
	/// <param name="data"> �Ώۃf�[�^�̔z��̃|�C���^ </param>
	/// <param name="size"> �� </param>
	/// <returns></returns>
	template<IntegerByte4 T>
	constexpr std::uint64_t FNV1aHash64(const T* data, std::uint64_t size) {
		constexpr uint64_t FNV_PRIME = 1099511628211ULL;  // FNV�f�� (2^40 + 2^8 + 0xb3)
		constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;

		std::uint64_t hash = FNV_OFFSET_BASIS;

		if constexpr (std::endian::native == std::endian::big)
		{
			BytesSwap(data, size * sizeof(T));
		}

		for (size_t i = 0; i < size; ++i) {
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
}
