//
// Created by zrufo on 2022/6/29.
//

#ifndef EASYFFT_PLANFFTWFLOAT_H
#define EASYFFT_PLANFFTWFLOAT_H

#include "IPlanFFTW.hpp"
#include "fftw3.h"

class PlanFFTWFloat final: public IPlanFFTW<ComplexF, fftwf_plan_s>{
public:

    PlanFFTWFloat(FFTPlanConfig config,
                  ComplexF *data_in, size_t data_in_size,
                  ComplexF *data_out, size_t data_out_size);

    void execute() override;

    ~PlanFFTWFloat() override;

};


#endif //EASYFFT_PLANFFTWFLOAT_H
