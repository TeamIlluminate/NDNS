#pragma once
#include "SDLAudioManager.h"
#include "Settings.h"
#include <functional>
class AbstractSoundProcessor {
    public:
    virtual void ProcessSound(Sint16* samples, int len)=0;
};


class Volume : public AbstractSoundProcessor {
    public:
    Volume(std::function<float()> func) : GetVolume(func) {}
    virtual void ProcessSound(Sint16* samples, int len);
    private: 
    std::function<float()> GetVolume;
};
