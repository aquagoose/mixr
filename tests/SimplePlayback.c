#include <mixr/device.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MX_CHECK(result, operation) {\
    printf(operation"\n");\
    MxResult res = result;\
    if (res != MX_RESULT_OK) {\
        printf("mixr operation "operation" failed: %d\n", res);\
        return 1;\
    }\
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Please provide a file path!\n");
        return 2;
    }

    const char *path = argv[1];
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        printf("fopen failed.\n");
        return 1;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *data = malloc(size);
    fread(data, 1, size, file);
    fclose(file);

    MxDeviceInfo deviceInfo =
    {
        .sampleRate = 48000,
        .flags = MX_DEVICE_STOPPED
    };

    MxDevice *device;
    MX_CHECK(mxCreateDevice(&deviceInfo, &device), "Create device");

    MxBufferInfo bufferInfo = {};
    MxBuffer buffer;
    MX_CHECK(mxCreateBuffer(device, &bufferInfo, &buffer), "Create buffer");

    MX_CHECK(mxUpdateBuffer(device, buffer, data, size), "Update buffer");
    free(data);

    MxSourceInfo sourceInfo =
    {
        .type = MX_SOURCE_TYPE_PCM,
        .format = { .dataType = MX_DATA_TYPE_F32, .sampleRate = 44100, .channels = 2 }
    };

    MxSource source;
    MX_CHECK(mxCreateSource(device, &sourceInfo, &source), "Create source");

    for (int i = 0; i < 10; i++)
        MX_CHECK(mxSourceQueueBuffer(device, source, i), "Queue buffer");

    MX_CHECK(mxStartDevice(device), "Start device");

    sleep(60);

    mxDestroyDevice(device);
    return 0;
}
