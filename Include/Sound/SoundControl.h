#pragma once
#ifdef EUGENE_XAUDIO
#include "Xaudio2/Xa2SoundControl.h"
#elif EUGENE_ANDROID
#include "AAudio/AaSoundControl.h"
#endif