//
// Created by zrufo on 2022-05-26.
//

#ifndef RFFTC_PLAN_H
#define RFFTC_PLAN_H

#include <complex>
#include "include/easyFFT.h"
#include <vector>


#define FFTW_PLAN_PARAMS int rank, const int *n, int howmany, const int *inembed, int istride, \
    int idist, const int *onembed, int ostride, int odist, int sign, \
    unsigned flags

#define FFTW_PLAN_PARAMS_INPUT rank, n, howmany, in, inembed, istride, \
    idist, out, onembed, ostride, odist, sign, flags


class Plan{
public:
    Plan (PLAN_CONSTRUCTOR_BASE,
          void* data_in, uint64_t data_in_size,
          void* data_out, uint64_t data_out_size);
    virtual ~Plan();
    void execute();
protected:

    std::vector<int> shape;
    int numberBatches;
    FFT_SIGN sign;
    FFT_DEVICE device;


    void *originPlan = nullptr;
    void init();
    void chose_fftw_plan();

    const void* data_in;
    const uint64_t data_in_size;
    const void* data_out;
    const uint64_t data_out_size;
    virtual void destroy_cpu_plan()=0;
    virtual void destroy_gpu_plan()=0;
    virtual void cpu_plan_execute()=0;
    virtual void gpu_plan_execute()=0;

    virtual void *get_fftw_plan(FFTW_PLAN_PARAMS)=0;
};


#endif //RFFTC_PLAN_H
