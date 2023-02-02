#pragma once
#include <concepts>

namespace Eugene
{

	// コンセプトを使って整数型と浮動小数点数のみテンプレート引数に入れれるように
	template<class T>
	concept ValueC = std::integral<T> || std::floating_point<T>;
}
