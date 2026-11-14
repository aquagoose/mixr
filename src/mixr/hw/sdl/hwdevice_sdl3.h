#ifndef MIXR_HWDEVICE_SDL3_H
#define MIXR_HWDEVICE_SDL3_H

#include "../hwdevice.h"

#include <SDL3/SDL.h>

typedef struct MxHWDevice_SDL3
{
    MxHWDevice base;
    SDL_AudioSpec spec;

    SDL_AudioStream *device;
} MxHWDevice_SDL3;

MxResult MxHWDevice_SDL3_init(const MxDeviceInfo *info, MxHWDevice **device);

#endif //MIXR_HWDEVICE_SDL3_H
