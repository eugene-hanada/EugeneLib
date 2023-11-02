#pragma once
#include <bitset>
#include "../ThirdParty/glm/glm/vec2.hpp"

namespace Eugene
{
	/// <summary>
	/// パッドの入力ID
	/// </summary>
	enum class PadID
	{
		/// <summary>
		/// 十字ボタン上
		/// </summary>
		UP,

		/// <summary>
		/// 十字ボタン下
		/// </summary>
		DOWN,

		/// <summary>
		/// 十字ボタン左
		/// </summary>
		LEFT,

		/// <summary>
		/// 十字ボタン右
		/// </summary>
		RIGHT,

		/// <summary>
		/// スタートボタン
		/// </summary>
		START,

		/// <summary>
		/// バックボタン
		/// </summary>
		BACK,

		/// <summary>
		/// 左スティック
		/// </summary>
		LEFT_THUMB,

		/// <summary>
		/// 右スティック
		/// </summary>
		RIGHT_THUMB,

		/// <summary>
		/// 左トリガー
		/// </summary>
		LEFT_TRIGGER,

		/// <summary>
		/// 右トリガー
		/// </summary>
		RIGHT_TRIGGER,

		/// <summary>
		/// 予約領域1
		/// </summary>
		RESERVE_1,

		/// <summary>
		/// 予約領域2
		/// </summary>
		RESERVE_2,

		/// <summary>
		/// A
		/// </summary>
		A,

		/// <summary>
		/// B
		/// </summary>
		B,

		/// <summary>
		/// X
		/// </summary>
		X,

		/// <summary>
		/// Y
		/// </summary>
		Y
	};

	constexpr PadID begin(PadID id)
	{
		return static_cast<PadID>(0);
	}

	constexpr PadID end(PadID id)
	{
		return PadID::Y;
	}

	constexpr PadID operator*(PadID id)
	{
		return id;
	}

	constexpr PadID operator++(PadID& id)
	{
		return id = PadID(std::underlying_type<PadID>::type(id) + 1);
	}

	/// <summary>
	/// ゲームパッドのデータ
	/// </summary>
	class GamePad
	{
	public:
		GamePad();

		/// <summary>
		/// 指定のIDが押されているか？
		/// </summary>
		/// <param name="id"> パッドの入力ID </param>
		/// <returns> 押されている時trueそうでないときfalse </returns>
		bool IsHit(PadID id) const;

		/// <summary>
		/// 左トリガーの値
		/// </summary>
		float leftTrigger_;

		/// <summary>
		/// 右トリガーの値
		/// </summary>
		float rightTrigger_;

		/// <summary>
		/// 左スティックの傾き
		/// </summary>
		glm::vec2 leftThumb_;

		/// <summary>
		/// 右スティックの傾き
		/// </summary>
		glm::vec2 rightThumb_;

		/// <summary>
		/// ボタンデータ
		/// </summary>
		std::bitset<16> buttons;
	};

}