#pragma once
#include <array>
#include "../Math/Vector2.h"

namespace Eugene
{
	enum class PadID
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		START,
		BACK,
		LEFT_THUMB,
		RIGHT_THUMB,
		LEFT_TRIGGER,
		RIGHT_TRIGGER,
		RESERVE_1,
		RESERVE_2,
		A,
		B,
		X,
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

	class GamePad
	{
	public:
		GamePad();
		bool IsHit(PadID id) const;
		float leftTrigger_;
		float rightTrigger_;
		Vector2 leftThumb_;
		Vector2 rightThumb_;
		std::array<bool, 16> buttons;
	};
}