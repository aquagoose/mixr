#include "hwdevice_sdl3.h"

#include <stdlib.h>

void MxHWDevice_SDL3_deinit(MxHWDevice *self)
{
    MxHWDevice_SDL3 *dev = (MxHWDevice_SDL3 *) self;

    if (dev->device)
        SDL_DestroyAudioStream(dev->device);

    free(dev);
}

static const MxHWDevice_Vtbl MxHWDevice_SDL3_Vtbl = { .deinit = MxHWDevice_SDL3_deinit };

MxResult MxHWDevice_SDL3_init(const MxDeviceInfo *info, MxHWDevice **device)
{
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.freq = (int) info->sampleRate;
    spec.channels = 2;

    MxHWDevice_SDL3 *dev = malloc(sizeof(MxHWDevice_SDL3));
    dev->base.vtbl = &MxHWDevice_SDL3_Vtbl;
    dev->spec = spec;
    dev->device = NULL;

    *device = (MxHWDevice *) dev;
    return MX_RESULT_OK;
}
