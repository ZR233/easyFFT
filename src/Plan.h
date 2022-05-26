//
// Created by zrufo on 2022-05-26.
//

#ifndef RFFTC_PLAN_H
#define RFFTC_PLAN_H

#include <complex>
#include "include/rfft.h"
#include <vector>

#define PLAN_CONSTRUCTOR_PARAMS (int FFTDim, const int *shape, int numberBatches, RFFT_PlanType planType, RFFT_SIGN sign, FFT_DEVICE device)


class Plan{
public:
    Plan PLAN_CONSTRUCTOR_PARAMS;
    virtual ~Plan();

protected:
    RFFT_PlanType planType;
    std::vector<int> shape;
    int numberBatches;
    RFFT_SIGN sign;
    FFT_DEVICE device;
    void * originPlan= nullptr;

    void chose_fftw_plan();
    virtual void* getDataIn()=0;
    virtual void* getDataOut()=0;
    void destroy_cpu_plan();
    void destroy_gpu_plan();
};


#endif //RFFTC_PLAN_H
