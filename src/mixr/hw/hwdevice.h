#ifndef MIXR_HWDEVICE_H
#define MIXR_HWDEVICE_H

#include "mixr/common.h"
#include "mixr/device.h"

typedef struct MxHWDevice MxHWDevice;

typedef struct MxHWDevice_Vtbl
{
    void (*deinit)(MxHWDevice *self);
    MxResult (*start)(MxHWDevice *self);
    void (*stop)(MxHWDevice *self);
} MxHWDevice_Vtbl;

struct MxHWDevice
{
    MxHWDevice_Vtbl *vtbl;
};

MxResult MxHWDevice_init(const MxDeviceInfo *info, MxDevice *device, MxHWDevice **hwDevice);
void MxHWDevice_deinit(MxHWDevice *device);

static MxResult MxHWDevice_start(MxHWDevice *device) { return device->vtbl->start(device); }
static void MxHWDevice_stop(MxHWDevice *device) { device->vtbl->stop(device); }

#endif //MIXR_HWDEVICE_H
