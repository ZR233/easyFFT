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


#define PLAN_CONSTRUCTOR_BASE int32_t dim, const int32_t *shape, int32_t number_batches, enum FFT_SIGN sign, enum FFT_DEVICE device
#define PLAN_CONSTRUCTOR_BASE_INPUT dim, shape, number_batches, sign, device


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

struct FFTPlanConfig{
    int32_t dim;
    const int32_t *shape;
    int32_t number_batches;
    enum FFT_SIGN sign;
    enum FFT_DEVICE device;
};
struct FFTPlanFloat {
    struct FFTPlanConfig config;
    void* ptr;
};
struct FFTPlanDouble {
    struct FFTPlanConfig config;
    void* ptr;
};
struct FFTPlanDoubleR2C {
    struct FFTPlanConfig config;
    void* ptr;
};
typedef float ComplexF[2];
typedef double ComplexD[2];
typedef float ComplexL[2];

EXT enum FFT_ERROR_CODE fft_planf_init(struct FFTPlanFloat *plan,
                                      ComplexF *in_complex, uint64_t in_size,
                                      ComplexF *out_complex, uint64_t out_size);

EXT enum FFT_ERROR_CODE fft_planf_device_name(struct FFTPlanFloat *plan, char *name, int size);
EXT void fft_close_planf(struct FFTPlanFloat *plan);
EXT void fft_close_plan(struct FFTPlanDouble *plan);
EXT enum FFT_ERROR_CODE fft_planf_execute(struct FFTPlanFloat *plan);

#ifdef __cplusplus
}
#endif


#endif //RFFT_RFFT_H


