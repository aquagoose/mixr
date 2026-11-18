#include "mixr/device.h"
#include "hw/hwdevice.h"

#include <stdlib.h>
#include <stdbool.h>

// Initial capacity of buffers and sources.
#define MX_INITIAL_CAPACITY 16

typedef struct MxBuffer_Impl
{
    void *data;
    // to avoid reallocating whenever the data is updated, we treat the data ptr like a list
    // reusing the ptr unless the submitted data is larger than the capacity
    size_t dataLength;
    size_t dataCapacity;
} MxBuffer_Impl;

typedef struct MxDevice_Impl
{
    MxDeviceInfo info;
    MxHWDevice *hwDevice;

    MxBuffer_Impl *buffers;
    size_t buffersLength;
    size_t buffersCapacity;
} MxDevice_Impl;

MxResult mxCreateDevice(const MxDeviceInfo *info, MxDevice **device)
{
    MxDevice_Impl *impl = malloc(sizeof(MxDevice_Impl));
    if (!impl)
        return MX_RESULT_OUT_OF_MEMORY;

    impl->info = *info;

    MxResult res = MxHWDevice_init(info, &impl->hwDevice);
    if (res != MX_RESULT_OK)
    {
        free(impl);
        return res;
    }

    impl->buffers = malloc(MX_INITIAL_CAPACITY * sizeof(MxBuffer_Impl));
    if (!impl->buffers)
    {
        MxHWDevice_deinit(impl->hwDevice);
        free(impl);
        return MX_RESULT_OUT_OF_MEMORY;
    }
    impl->buffersLength = 0;
    impl->buffersCapacity = MX_INITIAL_CAPACITY;

    *device = (MxDevice *) impl;
    return MX_RESULT_OK;
}

void mxDestroyDevice(MxDevice *device)
{
    if (!device)
        return;
    MxDevice_Impl *impl = (MxDevice_Impl *) device;

    free(impl->buffers);

    MxHWDevice_deinit(impl->hwDevice);
    free(impl);
}

MxResult mxStartDevice(MxDevice *device)
{
    MxDevice_Impl *impl = (MxDevice_Impl *) device;
    return MxHWDevice_start(impl->hwDevice);
}

void mxStopDevice(MxDevice *device)
{
    MxDevice_Impl *impl = (MxDevice_Impl *) device;
    MxHWDevice_stop(impl->hwDevice);
}

MxResult mxCreateBuffer(MxDevice *device, const MxBufferInfo *info, MxBuffer *buffer)
{
    MxDevice_Impl *impl = (MxDevice_Impl *) device;

    // resize the buffers list if need be
    if (impl->buffersLength + 1 > impl->buffersCapacity)
    {
        size_t newBuffersCapacity = impl->buffersCapacity << 1;
        MxBuffer_Impl *newBuffers = realloc(impl->buffers, newBuffersCapacity * sizeof(MxBuffer_Impl));
        if (!newBuffers)
            return MX_RESULT_OUT_OF_MEMORY;
        impl->buffers = newBuffers;
        impl->buffersCapacity = newBuffersCapacity;
    }

    size_t id = impl->buffersLength++;

    MxBuffer_Impl buf;
    buf.data = NULL;
    buf.dataLength = 0;
    buf.dataCapacity = 0;

    impl->buffers[id] = buf;

    *buffer = id;
    return MX_RESULT_OK;
}

MxResult mxUpdateBuffer(MxDevice* device, MxBuffer buffer, void* data, size_t dataLength)
{
    MxDevice_Impl *impl = (MxDevice_Impl *) device;

}
