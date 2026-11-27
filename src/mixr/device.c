#include "mixr/device.h"

#include <assert.h>

#include "hw/hwdevice.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Initial capacity of buffers and sources.
#define MX_INITIAL_CAPACITY 16

#define MX_GET_SAMPLE(out, type, buf, idx) {\
    uint32_t value = ((buf[idx + 0]) | (buf[idx + 1] << 8) | (buf[idx + 2] << 16) | (buf[idx + 3] << 24));\
    out = *(float *) &value;\
}

typedef struct MxBuffer_Impl
{
    void *data;
    // to avoid reallocating whenever the data is updated, we treat the data ptr like a list
    // reusing the ptr unless the submitted data is larger than the capacity
    size_t dataLength;
    size_t dataCapacity;

    bool valid; // determines if the buffer is valid, and is in-use currently.
} MxBuffer_Impl;

typedef struct MxSource_Impl
{
    MxAudioFormat format;

    // queue containing buffer IDs. it can't contain a reference to the buffers directly, as if the device's buffer
    // array is resized, the values in here may become invalid references.
    MxBuffer *queue;
    size_t queueCapacity;
    size_t queueFront;
    size_t queueBack;

    // set based on the source's sample rate vs the devices's sample rate.
    // for example, a 44.1khz source created on a 48khz device will have a base speed of ~0.92
    float baseSpeed;

    bool playing; // if the source shoudl be playing
    size_t pos; // the position, in samples
    float finePos; // the fine position in samples

    bool valid; // determisn if the source is valid, and is in-use currently
} MxSource_Impl;

typedef struct MxDevice_Impl
{
    MxDeviceInfo info;
    MxHWDevice *hwDevice;

    MxBuffer_Impl *buffers;
    size_t buffersLength;
    size_t buffersCapacity;

    MxSource_Impl *sources;
    size_t sourcesLength;
    size_t sourcesCapacity;
} MxDevice_Impl;

MxResult mxCreateDevice(const MxDeviceInfo *info, MxDevice **device)
{
    MxDevice_Impl *impl = malloc(sizeof(MxDevice_Impl));
    if (!impl)
        return MX_RESULT_OUT_OF_MEMORY;

    impl->info = *info;

    MxResult res = MxHWDevice_init(info, (MxDevice *) impl, &impl->hwDevice);
    if (res != MX_RESULT_OK)
    {
        mxDestroyDevice((MxDevice *) impl);
        return res;
    }

    impl->buffers = malloc(MX_INITIAL_CAPACITY * sizeof(MxBuffer_Impl));
    if (!impl->buffers)
    {
        mxDestroyDevice((MxDevice *) impl);
        return MX_RESULT_OUT_OF_MEMORY;
    }
    impl->buffersLength = 0;
    impl->buffersCapacity = MX_INITIAL_CAPACITY;

    impl->sources = malloc(MX_INITIAL_CAPACITY * sizeof(MxSource_Impl));
    if (!impl->sources)
    {
        mxDestroyDevice((MxDevice *) impl);
        return MX_RESULT_OUT_OF_MEMORY;
    }
    impl->sourcesLength = 0;
    impl->sourcesCapacity = MX_INITIAL_CAPACITY;

    *device = (MxDevice *) impl;
    return MX_RESULT_OK;
}

void mxDestroyDevice(MxDevice *device)
{
    if (!device)
        return;
    MxDevice_Impl *impl = (MxDevice_Impl *) device;

    if (impl->sources)
    {
        for (size_t i = 0; i < impl->sourcesLength; i++)
            free(impl->sources[i].queue);
    }
    free(impl->sources);

    if (impl->buffers)
    {
        for (size_t i = 0; i < impl->buffersLength; i++)
            free(impl->buffers[i].data);
    }
    free(impl->buffers);

    if (impl->hwDevice)
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
    buf.valid = true;

    impl->buffers[id] = buf;

    *buffer = id;
    return MX_RESULT_OK;
}

MxResult mxUpdateBuffer(MxDevice *device, MxBuffer buffer, void *data, size_t dataSize)
{
    // check that user is not attempting to copy over null data
    if (dataSize > 0 && !data)
        return MX_RESULT_INVALID_PARAMETER;

    MxDevice_Impl *impl = (MxDevice_Impl *) device;
    if (buffer >= impl->buffersLength)
        return MX_RESULT_INVALID_BUFFER;

    MxBuffer_Impl *buf = &impl->buffers[buffer];
    if (!buf->valid)
        return MX_RESULT_INVALID_BUFFER;

    if (!data || dataSize == 0)
        buf->dataLength = 0;
    else
    {
        // resize the buffer's data if required
        // this will also create the data ptr if the buffer is empty
        if (dataSize > buf->dataCapacity)
        {
            void *bufferData = realloc(buf->data, dataSize);
            if (!bufferData)
                return MX_RESULT_OUT_OF_MEMORY;
            buf->data = bufferData;
            buf->dataCapacity = dataSize;
        }

        memcpy(buf->data, data, dataSize);
        buf->dataLength = dataSize;
    }

    return MX_RESULT_OK;
}

MxResult mxCreateSource(MxDevice *device, const MxSourceInfo *info, MxSource *source)
{
    if (!device || !info)
        return MX_RESULT_INVALID_PARAMETER;

    MxDevice_Impl *impl = (MxDevice_Impl *) device;

    // resize the sources list if necessary
    if (impl->sourcesLength + 1 > impl->sourcesCapacity)
    {
        size_t newSourcesCapacity = impl->sourcesCapacity << 1;
        MxSource_Impl *newSources = realloc(impl->sources, newSourcesCapacity);
        if (!newSources)
            return MX_RESULT_OUT_OF_MEMORY;
        impl->sources = newSources;
        impl->sourcesCapacity = newSourcesCapacity;
    }

    size_t id = impl->sourcesLength++;

    MxSource_Impl src;
    src.valid = true;
    src.format = info->format;

    src.queue = malloc(MX_INITIAL_CAPACITY * sizeof(MxBuffer));
    src.queueCapacity = MX_INITIAL_CAPACITY;
    src.queueFront = 0;
    src.queueBack = (size_t) -1;

    src.baseSpeed = (float) src.format.sampleRate / (float) impl->info.sampleRate;

    src.pos = 0;
    src.finePos = 0.0f;

    impl->sources[id] = src;

    *source = id;
    return MX_RESULT_OK;
}

MxResult mxSourceQueueBuffer(MxDevice *device, MxSource source, MxBuffer buffer)
{
    if (!device)
        return MX_RESULT_INVALID_PARAMETER;

    MxDevice_Impl *impl = (MxDevice_Impl *) device;
    if (source > impl->sourcesLength)
        return MX_RESULT_INVALID_SOURCE;
    if (buffer > impl->buffersLength)
        return MX_RESULT_INVALID_BUFFER;

    MxSource_Impl *src = &impl->sources[source];
    if (!src->valid)
        return MX_RESULT_INVALID_SOURCE;
    MxBuffer_Impl *buf = &impl->buffers[buffer];
    if (!buf->valid)
        return MX_RESULT_INVALID_BUFFER;

    size_t queueLength;
    if (src->queueBack < src->queueFront)
        queueLength = (src->queueCapacity - (src->queueFront - src->queueBack)) + 1;
    else
        queueLength = (src->queueBack - src->queueFront) + 1;

    // queue is at capacity, needs resizing
    if (queueLength + 1 > src->queueCapacity)
    {
        printf("resize\n");

        size_t newQueueCapacity = src->queueCapacity << 1;
        MxBuffer *newQueue = malloc(newQueueCapacity * sizeof(MxBuffer));
        if (!newQueue)
            return MX_RESULT_OUT_OF_MEMORY;

        // since we're creating a new queue, we want to copy all the data from the old queue to the new one
        // however we must reset the queue to be "in order", and remove all wrap-around.
        size_t index = src->queueFront;
        size_t copyIndex = 0;
        do
        {
            newQueue[copyIndex++] = src->queue[index];
            if (++index > queueLength)
                index = 0;
        } while (index != src->queueFront);

        free(src->queue);
        src->queue = newQueue;
        src->queueCapacity = newQueueCapacity;
        src->queueFront = 0;
    }

    src->queueBack++;
    if (src->queueBack > src->queueCapacity)
        src->queueBack = 0;

    src->queue[src->queueBack] = buffer;

    for (int i = 0; i < queueLength; i++)
        printf("%lu\n", src->queue[i]);

    return MX_RESULT_OK;
}

MxResult mxSourcePlay(MxDevice *device, MxSource source)
{
    if (!device)
        return MX_RESULT_INVALID_PARAMETER;

    MxDevice_Impl *impl = (MxDevice_Impl *) device;
    if (source > impl->sourcesLength)
        return MX_RESULT_INVALID_SOURCE;

    MxSource_Impl *src = &impl->sources[source];
    if (!src->valid)
        return MX_RESULT_INVALID_SOURCE;

    src->playing = true;
    return MX_RESULT_OK;
}

MxResult mxMixStereoF32(MxDevice *device, float *buffer, size_t bufferLength)
{
    // don't write to a 0-length buffer.
    if (bufferLength == 0)
        return MX_RESULT_OK;
    if (!buffer)
        return MX_RESULT_INVALID_PARAMETER;

    // check that the buffer is divisible by 2.
    // todo profile this, maybe make this an assert. maybe
    if ((bufferLength % 2) != 0)
        return MX_RESULT_INVALID_PARAMETER;

    if (!device)
        return MX_RESULT_INVALID_PARAMETER;
    MxDevice_Impl *impl = (MxDevice_Impl *) device;

    MxBuffer_Impl *buffers = impl->buffers;
    size_t buffersLength = impl->buffersLength;
    MxSource_Impl *sources = impl->sources;
    size_t sourcesLength = impl->sourcesLength;

    for (size_t bufidx = 0; bufidx < bufferLength; bufidx += 2)
    {
        buffer[bufidx + 0] = 0;
        buffer[bufidx + 1] = 0;

        for (size_t s = 0; s < sourcesLength; s++)
        {
            MxSource_Impl *src = &sources[s];
            // todo source playing list
            if (!src->playing)
                continue;

            MxAudioFormat *format = &src->format;

            MxBuffer b = src->queue[src->queueFront];
            assert(b < buffersLength);
            MxBuffer_Impl *buf = &buffers[b];
            assert(buf->valid);

            uint8_t *bufData = buf->data;

            size_t bytePos = src->pos * 8;

            float l, r;
            MX_GET_SAMPLE(l, format.dataType, bufData, bytePos);
            MX_GET_SAMPLE(r, format.dataType, bufData, bytePos + 4);

            src->finePos += src->baseSpeed;
            size_t intPos = (size_t) src->finePos;
            src->pos += intPos;
            src->finePos -= intPos;

            buffer[bufidx + 0] += l;
            buffer[bufidx + 1] += r;
        }
    }

    return MX_RESULT_OK;
}
