#pragma once
#include "SoundSpeaker.h"
#include <fstream>

namespace Eugene
{
    class SoundControl;

    class SoundStreamSpeaker :
        public SoundSpeaker
    {
    public:
        SoundStreamSpeaker(const float maxPitchRate);
        virtual ~SoundStreamSpeaker();
     protected:
         std::ifstream file_;
    };
}
