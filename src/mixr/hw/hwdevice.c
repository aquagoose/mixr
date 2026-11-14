#include "hwdevice.h"

#include "sdl/hwdevice_sdl3.h"

MxResult MxHWDevice_init(const MxDeviceInfo *info, MxHWDevice **device)
{
    return MxHWDevice_SDL3_init(info, device);
}

void MxHWDevice_deinit(MxHWDevice *device)
{
    if (!device)
        return;

    device->vtbl->deinit(device);
}
