#ifndef MIXR_HWDEVICE_H
#define MIXR_HWDEVICE_H

#include "mixr/common.h"
#include "mixr/device.h"

typedef struct MxHWDevice MxHWDevice;

typedef struct MxHWDevice_Vtbl
{
    void (*deinit)(MxHWDevice *self);
    void (*start)(MxHWDevice *self);
    void (*stop)(MxHWDevice *self);
} MxHWDevice_Vtbl;

struct MxHWDevice
{
    MxHWDevice_Vtbl vtbl;
};

MxResult MxHWDevice_init(const MxDeviceInfo *info, MxHWDevice **device);
void MxHWDevice_deinit(MxHWDevice *device);

#endif //MIXR_HWDEVICE_H
