#include <mixr/device.h>

#include <stdio.h>
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
    MxDeviceInfo deviceInfo =
    {
        .sampleRate = 48000
    };

    MxDevice *device;
    MX_CHECK(mxCreateDevice(&deviceInfo, &device), "Create device");

    sleep(5);

    mxDestroyDevice(device);
    return 0;
}