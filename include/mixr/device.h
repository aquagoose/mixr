#ifndef MIXR_DEVICE_H
#define MIXR_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

// todo
#define MIXR_API

#include "common.h"
#include <stdint.h>

typedef struct MxDevice MxDevice;

typedef struct MxDeviceInfo
{
    uint32_t sampleRate; // the sampling rate in hz
} MxDeviceInfo;

MxResult mxCreateDevice(const MxDeviceInfo *info, MxDevice **device);
void mxDestroyDevice(MxDevice *device);

#ifdef __cplusplus
}
#endif

#endif //MIXR_DEVICE_H
