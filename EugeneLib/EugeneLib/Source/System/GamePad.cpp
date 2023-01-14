#include "../../Include/System/GamePad.h"

EugeneLib::GamePad::GamePad()
{
	buttons.fill(false);
	leftTrigger_ = 0.0f;
	rightTrigger_ = 0.0f;
}

bool EugeneLib::GamePad::IsHit(PadID id) const
{
	return buttons[std::underlying_type<PadID>::type(id)];
}
