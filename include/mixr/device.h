#ifndef MIXR_DEVICE_H
#define MIXR_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

// todo
#define MIXR_API

#include "common.h"
#include <stdint.h>

typedef uint32_t MxDeviceFlags;

// do not start the device on creation
#define MX_DEVICE_STOPPED (1 << 0)

typedef struct MxDevice MxDevice;

typedef struct MxDeviceInfo
{
    uint32_t sampleRate; // the sampling rate in hz
    MxDeviceFlags flags; // flags describing the device
} MxDeviceInfo;

MxResult mxCreateDevice(const MxDeviceInfo *info, MxDevice **device);
void mxDestroyDevice(MxDevice *device);

MxResult mxStartDevice(MxDevice *device);
void mxStopDevice(MxDevice *device);

#ifdef __cplusplus
}
#endif

#endif //MIXR_DEVICE_H
