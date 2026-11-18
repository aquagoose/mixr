#include "hwdevice_sdl3.h"

#include <stdlib.h>

#define MX_BUFFER_SIZE 512
#define MX_MIN(a, b) (a < b ? a : b)

static void MxHWDevice_SDL3_mix(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
    MxDevice *device = userdata;

    float buffer[MX_BUFFER_SIZE];
    while (additional_amount > 0)
    {
        int total = MX_MIN(additional_amount, MX_BUFFER_SIZE);
        mxMixStereoF32(device, buffer, total / 4);
        SDL_PutAudioStreamData(stream, buffer, total);
        additional_amount -= total;
    }
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

    dev->device = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &dev->spec, MxHWDevice_SDL3_mix, dev->ctx);
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
    dev->device = NULL;
}

static const MxHWDevice_Vtbl MxHWDevice_SDL3_Vtbl = { .deinit = MxHWDevice_SDL3_deinit, .start = MxHWDevice_SDL3_start, .stop = MxHWDevice_SDL3_stop };

MxResult MxHWDevice_SDL3_init(const MxDeviceInfo *info, MxDevice *device, MxHWDevice **hwDevice)
{
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.freq = (int) info->sampleRate;
    spec.channels = 2;

    if (!SDL_Init(SDL_INIT_AUDIO))
        return MX_RESULT_UNKNOWN_ERROR;

    MxHWDevice_SDL3 *dev = malloc(sizeof(MxHWDevice_SDL3));
    dev->base.vtbl = &MxHWDevice_SDL3_Vtbl;
    dev->ctx = device;
    dev->spec = spec;
    dev->device = NULL;

    *hwDevice = (MxHWDevice *) dev;
    return MX_RESULT_OK;
}
