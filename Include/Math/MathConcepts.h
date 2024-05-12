#pragma once
#include <concepts>

namespace Eugene
{

	// コンセプトを使って整数型と浮動小数点数のみテンプレート引数に入れれるように
	template<class T>
	concept ValueC = std::integral<T> || std::floating_point<T>;

	template<class T>
	concept IntegerByte1 = std::same_as<T, std::uint8_t> || std::same_as<T, std::int8_t> || std::same_as<T, char8_t> || std::same_as<T, char>;

	template<class T>
	concept IntegerByte2 = std::same_as<T, std::uint16_t> || std::same_as<T, std::int16_t> || std::same_as<T, char16_t> || std::same_as<T, wchar_t>;

	template<class T>
	concept IntegerByte4 = std::same_as<T, std::uint32_t> || std::same_as<T, std::int32_t>;
}
