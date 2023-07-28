#include "../../Include/System/GamePad.h"

Eugene::GamePad::GamePad()
{
	buttons.reset();
	leftTrigger_ = 0.0f;
	rightTrigger_ = 0.0f;
}

bool Eugene::GamePad::IsHit(PadID id) const
{
	return buttons.test(std::underlying_type<PadID>::type(id));
}
