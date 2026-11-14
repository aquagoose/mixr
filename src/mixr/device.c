#include "mixr/device.h"

typedef struct MxDevice_Impl
{
    MxDeviceInfo info;
} MxDevice_Impl;

MxResult mxCreateDevice(const MxDeviceInfo* info, MxDevice** device)
{
    return MX_RESULT_OK;
}

void mxDestroyDevice(MxDevice* device)
{

}
