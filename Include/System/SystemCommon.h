#pragma once
#include <array>

namespace Eugene
{
	/// <summary>
	/// �L�[�{�[�h�̏��()
	/// </summary>
	using KeyDataSpan = std::span<bool, KEYID_MAX>;

	/// <summary>
	/// �L�[�R�[�h�̃e�[�u��
	/// </summary>
	using KeyCodeTable = std::array<int, KEYID_MAX>;
}
