#pragma once

#ifdef EUGENE_WINDOWS
#include "Windows/Dll.h"
#elif EUGENE_ANDROID
#include "Linux/SharedObject.h"
#endif