#pragma once
#include "SoundBase.h"
#include <fstream>

namespace Eugene
{
    class SoundControl;

    class SoundStreamSpeaker :
        public SoundBase
    {
    public:
        virtual ~SoundStreamSpeaker();
        virtual void Play(void) = 0;
        virtual void Stop(void) = 0;
        virtual bool IsEnd(void) const = 0;
        virtual void SetPitchRate(float rate) = 0;
        virtual void SetOutput(SoundControl& control) = 0;
     protected:
         std::ifstream file_;
    };
}
