#include "hwdevice.h"

MxResult MxHWDevice_Init(const MxDeviceInfo *info, MxHWDevice **device)
{
    return MX_RESULT_OK;
}

void MxHWDevice_deinit(MxHWDevice *device)
{
    if (!device)
        return;

    device->vtbl.deinit(device);
}
