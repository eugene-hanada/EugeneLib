#pragma once
#include <type_traits>

namespace Eugene
{

	/// <summary>
	/// キーボードのID
	/// </summary>
	enum class KeyID
	{
		TAB,
		CAPS_LOCK,
		LEFT_SHIFT,
		LEFT_CTRL,
		ESCAPE,
		SPACE,
		LEFT_ALT,
		RIGHT_ALT,
		BACK_SPACE,
		RETURN,
		RIGHT_SHIFT,
		RIGHT_CTRL,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		NUM_LOCK,
		NUMPAD_0,
		NUMPAD_1,
		NUMPAD_2,
		NUMPAD_3,
		NUMPAD_4,
		NUMPAD_5,
		NUMPAD_6,
		NUMPAD_7,
		NUMPAD_8,
		NUMPAD_9,
		NUMPAD_MULTIPLY,
		NUMPAD_ADD,
		NUMPAD_SUBTRACT,
		NUMPAD_DECIMAL,
		NUMPAD_DIV,
		Num0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		MINUS,
		CARET,
		EN,
		AT,
		SQUARE_BRACKETS_FRONT,
		SEMICOLON,
		COLON,
		SQUARE_BRACKETS_BACK,
		COMMA,
		PERIOD,
		SLASH,
		BACK_SLASH,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		MAX,
	};

	constexpr auto KEYID_MAX = std::underlying_type<KeyID>::type(KeyID::MAX);

	constexpr KeyID begin(KeyID id)
	{
		return static_cast<KeyID>(0);
	}

	constexpr KeyID end(KeyID id)
	{
		return KeyID::MAX;
	}

	constexpr KeyID operator*(KeyID id)
	{
		return id;
	}

	constexpr KeyID operator++(KeyID& id)
	{
		return id = KeyID(std::underlying_type<KeyID>::type(id) + 1);
	}

}