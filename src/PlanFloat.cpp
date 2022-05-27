//
// Created by zrufo on 2022-05-17.
//

#include "PlanFloat.h"
#include <iostream>
#include "PlanComplex.hpp"
#include "fftw3.h"



PlanFloat::PlanFloat(PLAN_CONSTRUCTOR_BASE,
                     float *in_complex, uint64_t in_size,
                     float *out_complex, uint64_t out_size):
    PlanComplex<float>(PLAN_CONSTRUCTOR_BASE_INPUT,
                       in_complex, in_size, out_complex, out_size) {

    std::cout<< "create PlanFloat"<< std::endl;
    init();
}

PlanFloat::~PlanFloat() {
    std::cout<< "destroy PlanFloat"<< std::endl;
}

void PlanFloat::destroy_cpu_plan() {
    if (originPlan != nullptr){
        std::cout<< "destroy fftwf_plan"<< std::endl;
        fftwf_destroy_plan((fftwf_plan) originPlan);
    }
}

void PlanFloat::destroy_gpu_plan() {

}



void *PlanFloat::get_fftw_plan(FFTW_PLAN_PARAMS) {
    auto in = (fftwf_complex*)data_in;
    auto out = (fftwf_complex*)data_out;

    auto plan = fftwf_plan_many_dft(FFTW_PLAN_PARAMS_INPUT);
    return plan;
}




