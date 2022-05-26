//
// Created by zrufo on 2022-05-15.
//

#ifndef RFFT_RFFT_H
#define RFFT_RFFT_H

#include <stdint.h>

#ifdef WIN32
#define EXT __declspec(dllexport)
#else
#define EXT
#endif

#define FFT_PLAN_BODY { void* ptr;FFT_DEVICE device;};

#ifdef __cplusplus
extern "C" {
#endif

enum RFFT_ErrorCode{
    RFFT_ErrorCode_OK,
    RFFT_ErrorCode_PLAN_TYPE
};

enum RFFT_PlanType{
    RFFT_PLAN_TYPE_COMPLEX_FLOAT,
    RFFT_PLAN_TYPE_COMPLEX_DOUBLE
};
enum RFFT_SIGN{
    RFFT_SIGN_FORWARD,
    RFFT_SIGN_BACKWARD,
};

enum FFT_DEVICE{
    FFT_DEVICE_CPU,
    FFT_DEVICE_GPU,
};

struct FFTPlanFloat FFT_PLAN_BODY
struct FFTPlanDouble FFT_PLAN_BODY
struct FFTPlanDoubleR2C FFT_PLAN_BODY



EXT uint64_t createPlan(RFFT_PlanType type, RFFT_ErrorCode* err);
EXT void closePlan(uint64_t ptr);

#ifdef __cplusplus
}
#endif


#endif //RFFT_RFFT_H


