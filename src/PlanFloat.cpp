//
// Created by zrufo on 2022-05-17.
//

#include "PlanFloat.h"
#include <iostream>
#include "PlanComplex.hpp"



PlanFloat::PlanFloat(int FFTDim, const int *shape, int numberBatches, RFFT_SIGN sign, FFT_DEVICE device):
    PlanComplex<float>(FFTDim, shape, numberBatches, RFFT_PLAN_TYPE_COMPLEX_FLOAT, sign, device) {
//    fftw_plan_dft_1d()

    std::cout<< "create"<< std::endl;
}

PlanFloat::~PlanFloat() {
    std::cout<< "destroy"<< std::endl;
}




