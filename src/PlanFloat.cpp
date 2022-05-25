//
// Created by zrufo on 2022-05-17.
//

#include "PlanFloat.h"
#include "fftw3.h"
#include <iostream>
#include <vector>
#include <numeric>



Plan::Plan(int FFTDim, const int *shape, int numberBatches, RFFT_PlanType planType, RFFT_SIGN sign) {
    this->planType = planType;
    this->shape.assign(FFTDim, 0);
    this->sign = sign;
    for (int i = 0; i < FFTDim; ++i) {
        this->shape[i] = shape[i];
    }

    this->numberBatches=numberBatches;
    isUseFFTW = numberBatches < 5;

    if (isUseFFTW){
        chose_fftw_plan();
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
    if (isUseFFTW){
        if (originPlan != nullptr){
            switch (planType) {
                case RFFT_PLAN_TYPE_COMPLEX_FLOAT:
                    fftwf_destroy_plan((fftwf_plan) originPlan);
                    break;
            }
        }
    }
}


PlanFloat::PlanFloat(int FFTDim, const int *shape, int numberBatches, RFFT_SIGN sign):
    Plan(FFTDim, shape, numberBatches, RFFT_PLAN_TYPE_COMPLEX_FLOAT, sign) {
//    fftw_plan_dft_1d()

    std::cout<< "create"<< std::endl;
}

PlanFloat::~PlanFloat() {
    std::cout<< "destroy"<< std::endl;
}

void *PlanFloat::getDataIn() {
    return nullptr;
}

void *PlanFloat::getDataOut() {
    return nullptr;
}


