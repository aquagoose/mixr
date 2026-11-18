#include "hwdevice_sdl3.h"

#include <stdlib.h>

static void MxHWDevice_SDL3_mix(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{

}

static void MxHWDevice_SDL3_deinit(MxHWDevice *self)
{
    MxHWDevice_SDL3 *dev = (MxHWDevice_SDL3 *) self;

    if (dev->device)
        SDL_DestroyAudioStream(dev->device);

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    free(dev);
}

static MxResult MxHWDevice_SDL3_start(MxHWDevice *self)
{
    MxHWDevice_SDL3 *dev = (MxHWDevice_SDL3 *) self;
    // no need to start the device if it already exists
    if (dev->device)
        return MX_RESULT_OK;

    dev->device = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &dev->spec, MxHWDevice_SDL3_mix, NULL);
    if (!dev->device)
        return MX_RESULT_UNKNOWN_ERROR;

    if (!SDL_ResumeAudioStreamDevice(dev->device))
        return MX_RESULT_UNKNOWN_ERROR;

    return MX_RESULT_OK;
}

static void MxHWDevice_SDL3_stop(MxHWDevice *self)
{
    MxHWDevice_SDL3 *dev = (MxHWDevice_SDL3 *) self;
    if (!dev->device)
        return;

    SDL_DestroyAudioStream(dev->device);
}

static const MxHWDevice_Vtbl MxHWDevice_SDL3_Vtbl = { .deinit = MxHWDevice_SDL3_deinit, .start = MxHWDevice_SDL3_start, .stop = MxHWDevice_SDL3_stop };

MxResult MxHWDevice_SDL3_init(const MxDeviceInfo *info, MxHWDevice **device)
{
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.freq = (int) info->sampleRate;
    spec.channels = 2;

    if (!SDL_Init(SDL_INIT_AUDIO))
        return MX_RESULT_UNKNOWN_ERROR;

    MxHWDevice_SDL3 *dev = malloc(sizeof(MxHWDevice_SDL3));
    dev->base.vtbl = &MxHWDevice_SDL3_Vtbl;
    dev->spec = spec;
    dev->device = NULL;

    *device = (MxHWDevice *) dev;
    return MX_RESULT_OK;
}
