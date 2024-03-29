#include "SoundProcessing.h"

void Volume::ProcessSound(Sint16 *samples, int len)
{
    float volume_setting = GetVolume();
    Sint16 *sample = &samples[0];
    for (int i = 0; i < len; ++i)
    {
        *sample *= volume_setting;
        sample++;
    }
}