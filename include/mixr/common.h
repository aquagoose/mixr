#ifndef MIXR_COMMON_H
#define MIXR_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MxResult
{
    MX_RESULT_OK, // success
    MX_RESULT_UNKNOWN_ERROR, // unknown/undocumented error
    MX_RESULT_OUT_OF_MEMORY, // a memory allocation failed
} MxResult;

#ifdef __cplusplus
}
#endif

#endif //MIXR_COMMON_H
