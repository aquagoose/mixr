#include "hwdevice.h"

#include "sdl/hwdevice_sdl3.h"

MxResult MxHWDevice_init(const MxDeviceInfo *info, MxHWDevice **device)
{
    MxHWDevice *dev;

    MxResult res = MxHWDevice_SDL3_init(info, &dev);
    if (res != MX_RESULT_OK)
        return res;

    if ((info->flags & MX_DEVICE_STOPPED) == 0 && ((res = dev->vtbl->start(dev))) != MX_RESULT_OK)
        return res;

    *device = dev;
    return MX_RESULT_OK;
}

void MxHWDevice_deinit(MxHWDevice *device)
{
    if (!device)
        return;

    device->vtbl->deinit(device);
}
