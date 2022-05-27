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

#define FFT_PLAN_BODY void* ptr;
#define PLAN_CONSTRUCTOR_BASE int FFTDim, const int *shape, int numberBatches, FFT_SIGN sign, FFT_DEVICE device
#define PLAN_CONSTRUCTOR_BASE_INPUT FFTDim, shape, numberBatches, sign, device


#ifdef __cplusplus
extern "C" {
#endif

enum FFT_ErrorCode {
    FFT_ErrorCode_OK,
    FFT_ErrorCode_PLAN_TYPE
};

enum FFT_SIGN {
    FFT_SIGN_FORWARD,
    FFT_SIGN_BACKWARD,
};

enum FFT_DEVICE {
    FFT_DEVICE_CPU,
    FFT_DEVICE_GPU,
};


struct FFTPlanFloat {
    FFT_PLAN_BODY
};
struct FFTPlanDouble {
    FFT_PLAN_BODY
};
struct FFTPlanDoubleR2C {
    FFT_PLAN_BODY
};


EXT FFTPlanFloat fft_new_plan_float(PLAN_CONSTRUCTOR_BASE,
                                    float *in_complex, uint64_t in_size,
                                    float *out_complex, uint64_t out_size,
                                    FFT_ErrorCode *err);


EXT void fft_close_plan(FFTPlanFloat plan);
EXT FFTPlanFloat fft_execute(PLAN_CONSTRUCTOR_BASE, FFT_ErrorCode *err);

#ifdef __cplusplus
}
#endif


#endif //RFFT_RFFT_H


