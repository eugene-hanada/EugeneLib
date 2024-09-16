#pragma once
#ifdef EUGENE_XAUDIO
#include "Xaudio2/Xa2SoundSpeaker.h"
#elif EUGENE_ANDROID
#include "AAudio/AaSoundSpeaker.h"
#endif
