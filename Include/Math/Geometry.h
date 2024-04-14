#pragma once
#include <algorithm>
#include <bit>
#include "../ThirdParty/glm/glm/glm.hpp"
#include "../ThirdParty/glm/glm/ext.hpp"
#include "MathConcepts.h"
#include "Random.h"

template<class T>
concept IntegerByte1 = std::same_as<T, std::uint8_t> || std::same_as<T, std::int8_t> || std::same_as<T, char8_t>;

template<class T>
concept IntegerByte2 = std::same_as<T, std::uint16_t> || std::same_as<T, std::int16_t> || std::same_as<T, char16_t> || std::same_as<T, wchar_t>;

template<class T>
concept IntegerByte4 = std::same_as<T, std::uint32_t> || std::same_as<T, std::int32_t>;

namespace Eugene
{
	using namespace glm;

	/// <summary>
	/// Vector2Tmpのx=0,y=0を表す定数
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<ValueC T>
	constexpr vec<2,T> zeroVector2{ static_cast<T>(0),static_cast<T>(0) };

	/// <summary>
	/// Vector2Tmpの上方向x=0,y=-1を表す定数
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<ValueC T>
	constexpr vec<2, T> upVector2{ static_cast<T>(0),static_cast<T>(-1) };

	/// <summary>
	/// Vector2Tmpの下方向x=0,y=1を表す定数
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<ValueC T>
	constexpr vec<2, T> downVector2{ static_cast<T>(0),static_cast<T>(1) };

	/// <summary>
	/// Vector2Tmpの右方向x=1,y=0を表す定数
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<ValueC T>
	constexpr vec<2, T> rightVector2{ static_cast<T>(1),static_cast<T>(0) };

	/// <summary>
	/// Vector2Tmpの上方向x=-1,y=0を表す定数
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<ValueC T>
	constexpr vec<2, T> leftVector2{ static_cast<T>(-1),static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> zeroVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> upVector3{ static_cast<T>(0),static_cast<T>(1) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> downVector3{ static_cast<T>(0),static_cast<T>(-1) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> forwardVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(1) };

	template<ValueC T>
	constexpr vec<3, T> backVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(-1) };

	template<ValueC T>
	constexpr vec<3, T> leftVector3{ static_cast<T>(-1),static_cast<T>(0) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> rightVector3{ static_cast<T>(1),static_cast<T>(0) ,static_cast<T>(0) };

	/// <summary>
	/// 二乗する
	/// </summary>
	/// <param name="val"> 二乗したい値 </param>
	/// <returns> 二乗した値 </returns>
	template<ValueC T>
	constexpr T Square(T val)
	{
		return val * val;
	}

	// 円周率
	template<ValueC T>
	constexpr T pi = static_cast<T>(3.1415926535897932384626433832795);

	// 円周率*2
	template<ValueC T>
	constexpr T pi2 = pi<T> *static_cast<T>(2.0);

	/// <summary>
	/// 度数法を弧度法にする
	/// </summary>
	/// <param name="deg"> 変換したい度数法の値 </param>
	/// <returns>  </returns>
	template<ValueC T>
	constexpr T Deg2Rad(const T& deg)
	{
		return deg * (pi<T> / static_cast<T>(180));
	}

	template<ValueC T>
	constexpr glm::vec<3,T> Deg2Rad(const glm::vec<3,T>& deg)
	{
		return { Deg2Rad(deg.x),Deg2Rad(deg.y) ,Deg2Rad(deg.z) };
	}

	/// <summary>
	/// 弧度法を度数法にする
	/// </summary>
	/// <param name="rad"></param>
	/// <returns></returns>
	template<ValueC T>
	constexpr T Rad2Deg(const T& rad)
	{
		return rad * (static_cast<T>(180) / pi<T>);
	}

	template<ValueC T>
	constexpr glm::vec<3,T> Rad2Deg(const glm::vec<3,T>& rad)
	{
		return { Rad2Deg(rad.x),Rad2Deg(rad.y) ,Rad2Deg(rad.z) };
	}

	/// <summary>
	/// 角度を指定範囲に正規化する
	/// </summary>
	/// <param name="angle"> 変換する値 </param>
	/// <param name="valMin"> 最小値(デフォルト=0) </param>
	/// <param name="valMax"> 最大値(デフォルト=pi2) </param>
	/// <returns></returns>
	float GetNormalizedAngle(float angle, float valMin = 0.0f, float valMax = pi2<float>);

	/// <summary>
	/// 角度の正規化(上のやつのdouble版)
	/// </summary>
	/// <param name="angle"></param>
	/// <param name="valMin"></param>
	/// <param name="valMax"></param>
	/// <returns></returns>
	double GetNormalizedAngle(double angle, double valMin = 0.0f, double valMax = pi2<double>);

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

	/// <summary>
	/// FNV1aでハッシュ値を生成する
	/// </summary>
	/// <typeparam name="T"> 1バイトの型 </typeparam>
	/// <param name="data"> 対象データの配列のポインタ </param>
	/// <param name="size"> 数 </param>
	/// <returns></returns>
	template<IntegerByte1 T>
	constexpr std::uint64_t FNV1aHash64(const T* data, std::uint64_t size) {
		constexpr uint64_t FNV_PRIME = 1099511628211ULL;  // FNV素数 (2^40 + 2^8 + 0xb3)
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
	/// FNV1aでハッシュ値を生成する
	/// </summary>
	/// <typeparam name="T"> 2バイトの型 </typeparam>
	/// <param name="data"> 対象データの配列のポインタ </param>
	/// <param name="size"> 数 </param>
	/// <returns></returns>
	template<IntegerByte2 T>
	constexpr std::uint64_t FNV1aHash64(const T* data, std::uint64_t size) {
		constexpr uint64_t FNV_PRIME = 1099511628211ULL;  // FNV素数 (2^40 + 2^8 + 0xb3)
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
	/// FNV1aでハッシュ値を生成する
	/// </summary>
	/// <typeparam name="T"> 4バイトの型 </typeparam>
	/// <param name="data"> 対象データの配列のポインタ </param>
	/// <param name="size"> 数 </param>
	/// <returns></returns>
	template<IntegerByte4 T>
	constexpr std::uint64_t FNV1aHash64(const T* data, std::uint64_t size) {
		constexpr uint64_t FNV_PRIME = 1099511628211ULL;  // FNV素数 (2^40 + 2^8 + 0xb3)
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
};