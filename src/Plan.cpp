//
// Created by zrufo on 2022-05-26.
//

#include "Plan.h"
#include "fftw3.h"
#include <iostream>


Plan::Plan (PLAN_CONSTRUCTOR_BASE,
            void* data_in, uint64_t data_in_size,
            void* data_out, uint64_t data_out_size):
    sign(sign),
    device(device),
    data_in(data_in),
    data_in_size(data_in_size),
    data_out(data_out),
    data_out_size(data_out_size)
{
    this->shape.assign(FFTDim, 0);

    for (int i = 0; i < FFTDim; ++i) {
        this->shape[i] = shape[i];
    }

    this->numberBatches=numberBatches;
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
        case FFT_SIGN_FORWARD:
            sign = FFTW_FORWARD;
            break;
        case FFT_SIGN_BACKWARD:
            sign = FFTW_BACKWARD;
            break;
    }

    originPlan = get_fftw_plan(
            (int)shape.size(),
                shape.data(),
                numberBatches,
                inembed, istride, idist,
                onembed, ostride, odist,
                sign,
                FFTW_MEASURE
            );
}


Plan::~Plan() {
    std::cout<< "destroy Plan"<< std::endl;
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
//    if (originPlan != nullptr){
//        switch (planType) {
//            case RFFT_PLAN_TYPE_COMPLEX_FLOAT:
//                fftwf_destroy_plan((fftwf_plan) originPlan);
//                break;
//            case RFFT_PLAN_TYPE_COMPLEX_DOUBLE:
//                fftw_destroy_plan((fftw_plan) originPlan);
//                break;
//
//        }
//    }
}

void Plan::destroy_gpu_plan() {

}

void Plan::init() {
    switch (device) {

        case FFT_DEVICE_CPU:
            chose_fftw_plan();
            break;
        case FFT_DEVICE_GPU:
            break;
    }
}
