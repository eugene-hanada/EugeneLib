#ifndef EUGENE_TEST
#include <mimalloc-new-delete.h>
#endif

#include "../../Include/System/System.h"
#include "../../Include/Graphics/GpuEngine.h"
#include "../../Include/Graphics/Graphics.h"

Eugene::Mouse::Mouse() :
	pos{0.0f,0.0f}
{
	flags.reset();
	flags.set(static_cast<size_t>(Flags::ShowCursor));
}


Eugene::TouchData::Touch::Touch() :
	pos{0.0f,0.0f}, nowTime{0.0f},downTime{0.0f}
{
}

Eugene::TouchData::TouchData() :
	touchCount_{0u}
{
}
