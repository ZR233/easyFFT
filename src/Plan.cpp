//
// Created by zrufo on 2022-05-26.
//

#include "Plan.h"
#include "fftw3.h"


Plan::Plan PLAN_CONSTRUCTOR_PARAMS:
    planType(planType),
    sign(sign),
    device(device)
{
    this->shape.assign(FFTDim, 0);

    for (int i = 0; i < FFTDim; ++i) {
        this->shape[i] = shape[i];
    }

    this->numberBatches=numberBatches;

    switch (device) {

        case FFT_DEVICE_CPU:
            chose_fftw_plan();
            break;
        case FFT_DEVICE_GPU:
            break;
    }

}

void Plan::chose_fftw_plan() {
    int *inembed = shape.data(), *onembed = shape.data();
    int odist = 1;
    for (auto one: shape) {
        odist *= one;
    }
    int idist = odist;
    int ostride =1, istride = 1;
    int sign;
    switch (this->sign) {
        case RFFT_SIGN_FORWARD:
            sign = FFTW_FORWARD;
            break;
        case RFFT_SIGN_BACKWARD:
            sign = FFTW_BACKWARD;
            break;
    }

    switch (planType) {
        case RFFT_PLAN_TYPE_COMPLEX_FLOAT:{
            auto dataIn =(fftwf_complex*) getDataIn();
            auto dataOut =(fftwf_complex*) getDataOut();

            originPlan = fftwf_plan_many_dft(
                    (int)shape.size(),
                    shape.data(),
                    numberBatches,
                    dataIn, inembed, istride, idist,
                    dataOut, onembed, ostride, odist,
                    sign,
                    FFTW_MEASURE
            );
            break;
        }
        case RFFT_PLAN_TYPE_COMPLEX_DOUBLE:
            break;
    }
}


Plan::~Plan() {
    switch (device) {
        case FFT_DEVICE_CPU:
            destroy_cpu_plan();
            break;
        case FFT_DEVICE_GPU:
            destroy_gpu_plan();
            break;
    }
}

void Plan::destroy_cpu_plan() {
    if (originPlan != nullptr){
        switch (planType) {
            case RFFT_PLAN_TYPE_COMPLEX_FLOAT:
                fftwf_destroy_plan((fftwf_plan) originPlan);
                break;
            case RFFT_PLAN_TYPE_COMPLEX_DOUBLE:
                fftw_destroy_plan((fftw_plan) originPlan);
                break;

        }
    }
}

void Plan::destroy_gpu_plan() {

}
