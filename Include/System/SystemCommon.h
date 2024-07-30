#pragma once
#include <array>

namespace Eugene
{
	/// <summary>
	/// キーボードの情報()
	/// </summary>
	using KeyDataSpan = std::span<bool, KEYID_MAX>;

	/// <summary>
	/// キーコードのテーブル
	/// </summary>
	using KeyCodeTable = std::array<int, KEYID_MAX>;
}
