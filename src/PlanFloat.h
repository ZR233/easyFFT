//
// Created by zrufo on 2022-05-17.
//

#ifndef RFFTC_PLANFLOAT_H
#define RFFTC_PLANFLOAT_H

#include <complex>
#include "include/rfft.h"
#include <vector>
#include "PlanComplex.hpp"




class PlanFloat: public PlanComplex<float>{
public:
    PlanFloat(int FFTDim, const int *shape, int numberBatches, RFFT_SIGN sign, FFT_DEVICE device);
    ~PlanFloat() override;

private:
    PlanFloat()=default;
};


#endif //RFFTC_PLANFLOAT_H
