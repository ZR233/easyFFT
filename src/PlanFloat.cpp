//
// Created by zrufo on 2022-05-17.
//

#include "PlanFloat.h"
#include <iostream>
#include "PlanComplex.hpp"
#include "fftw3.h"




PlanFloat::PlanFloat(FFTPlanConfig config,
                     ComplexF *in_complex, uint64_t in_size,
                     ComplexF *out_complex, uint64_t out_size):
    Plan<ComplexF>(config, FFTW_PLAN_TYPE_F,
                       in_complex, in_size, out_complex, out_size) {

    std::cout<< "create PlanFloat"<< std::endl;
}

PlanFloat::~PlanFloat() {
    std::cout<< "destroy PlanFloat"<< std::endl;
}


void *PlanFloat::get_fftw_plan(FFTW_PLAN_PARAMS) {
    device_name = "FFTW3f";
    auto in = data_in;
    auto out = data_out;
    auto plan = fftwf_plan_many_dft(FFTW_PLAN_PARAMS_INPUT);
    return plan;
}

void PlanFloat::cpu_plan_execute() {
    std::cout<< "destroy fftwf_plan"<< std::endl;
    auto plan = (fftwf_plan) originPlan;
    fftwf_execute(plan);
}






