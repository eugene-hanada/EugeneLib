#pragma once
#include <array>
#include <span>
#include "KeyID.h"

namespace Eugene
{
	/// <summary>
	/// キーボードデータのspan
	/// </summary>
	using KeyDataSpan = std::span<bool, KEYID_MAX>;

	/// <summary>
	/// キーコードのテーブルの型
	/// </summary>
	using KeyCodeTable = std::array<int, KEYID_MAX>;
}
