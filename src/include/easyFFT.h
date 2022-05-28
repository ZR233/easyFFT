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
#define PLAN_CONSTRUCTOR_BASE int FFTDim, const int *shape, int numberBatches, enum FFT_SIGN sign, enum FFT_DEVICE device
#define PLAN_CONSTRUCTOR_BASE_INPUT FFTDim, shape, numberBatches, sign, device


#ifdef __cplusplus
extern "C" {
#endif

enum FFT_ERROR_CODE {
    FFT_ERROR_CODE_OK,
    FFT_ERROR_CODE_IN_SIZE,
    FFT_ERROR_CODE_OUT_SIZE,
    FFT_ERROR_CODE_CL,
    FFT_ERROR_CODE_DIM_TOO_BIG,
    FFT_ERROR_CODE_NO_CL_DEVICE,
    FFT_ERROR_CODE_VKFFT,
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
typedef float ComplexF[2];
typedef double ComplexD[2];
typedef float ComplexL[2];

EXT struct FFTPlanFloat fft_new_plan_float(PLAN_CONSTRUCTOR_BASE,
                                    ComplexF *in_complex, uint64_t in_size,
                                    ComplexF *out_complex, uint64_t out_size,
                                    enum FFT_ERROR_CODE *err);

EXT enum FFT_ERROR_CODE fft_plan_device_name(struct FFTPlanFloat plan, char *name, int size);
EXT void fft_close_plan(struct FFTPlanFloat plan);
EXT enum FFT_ERROR_CODE fft_execute(struct FFTPlanFloat plan);

#ifdef __cplusplus
}
#endif


#endif //RFFT_RFFT_H


