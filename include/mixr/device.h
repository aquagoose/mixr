#ifndef MIXR_DEVICE_H
#define MIXR_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

// todo
#define MIXR_API

#include "common.h"

#include <stdint.h>
#include <stddef.h>

typedef uint32_t MxDeviceFlags;

// do not start the device on creation
#define MX_DEVICE_STOPPED (1 << 0)

typedef struct MxDevice MxDevice;
typedef size_t MxBuffer;
typedef size_t MxSource;

typedef enum MxSourceType
{
    MX_SOURCE_TYPE_PCM
} MxSourceType;

typedef struct MxDeviceInfo
{
    uint32_t sampleRate; // the sampling rate in hz
    MxDeviceFlags flags; // flags describing the device
} MxDeviceInfo;

typedef struct MxBufferInfo
{

} MxBufferInfo;

typedef struct MxSourceInfo
{
    MxSourceType type;
    MxAudioFormat format;
} MxSourceInfo;

MxResult mxCreateDevice(const MxDeviceInfo *info, MxDevice **device);
void mxDestroyDevice(MxDevice *device);

MxResult mxStartDevice(MxDevice *device);
void mxStopDevice(MxDevice *device);

MxResult mxCreateBuffer(MxDevice *device, const MxBufferInfo *info, MxBuffer *buffer);
MxResult mxUpdateBuffer(MxDevice *device, MxBuffer buffer, void* data, size_t dataSize);

MxResult mxCreateSource(MxDevice *device, const MxSourceInfo *info, MxSource *source);

/**
 * Mix to a stereo, floating-point buffer.
 * @param device The device to mix with.
 * @param buffer The float buffer to write to.
 * @param bufferLength The number of floats in the buffer. This value must be divisible by 2.
 */
MxResult mxMixStereoF32(MxDevice *device, float *buffer, size_t bufferLength);

#ifdef __cplusplus
}
#endif

#endif //MIXR_DEVICE_H
