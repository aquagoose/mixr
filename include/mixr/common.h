#ifndef MIXR_COMMON_H
#define MIXR_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum MxResult
{
    MX_RESULT_OK, // success
    MX_RESULT_UNKNOWN_ERROR, // unknown/undocumented error
    MX_RESULT_OUT_OF_MEMORY, // a memory allocation failed
    MX_RESULT_INVALID_PARAMETER, // a function parameter is invalid.
    MX_RESULT_INVALID_BUFFER, // the buffer is not valid
} MxResult;

typedef enum MxDataType
{
    MX_DATA_TYPE_U8, // unsigned byte
    MX_DATA_TYPE_I8, // signed byte
    MX_DATA_TYPE_I16, // signed short
    MX_DATA_TYPE_I32, // signed integer
    MX_DATA_TYPE_F32 // float
} MxDataType;

typedef struct MxAudioFormat
{
    MxDataType dataType; // the audio's data type
    uint32_t sampleRate; // the sampling rate in Hz
    uint8_t channels; // the number of channels, 1 = mono, 2 = stereo
} MxAudioFormat;

#ifdef __cplusplus
}
#endif

#endif //MIXR_COMMON_H
