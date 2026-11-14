#include "mixr/device.h"
#include "hw/hwdevice.h"

#include <stdlib.h>

typedef struct MxDevice_Impl
{
    MxDeviceInfo info;
    MxHWDevice *hwDevice;
} MxDevice_Impl;

MxResult mxCreateDevice(const MxDeviceInfo* info, MxDevice** device)
{
    MxDevice_Impl *impl = malloc(sizeof(MxDevice_Impl));
    if (!impl)
        return MX_RESULT_OUT_OF_MEMORY;

    impl->info = *info;

    *device = (MxDevice *) impl;
    return MX_RESULT_OK;
}

void mxDestroyDevice(MxDevice* device)
{
    if (!device)
        return;

    MxDevice_Impl *impl = (MxDevice_Impl *) device;
    free(impl);
}
