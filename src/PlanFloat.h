//
// Created by zrufo on 2022-05-17.
//

#ifndef RFFTC_PLANFLOAT_H
#define RFFTC_PLANFLOAT_H

#include <complex>
#include "include/easyFFT.h"
#include <vector>
#include "PlanComplex.hpp"


class PlanFloat : public PlanComplex<float> {
public:
    PlanFloat(PLAN_CONSTRUCTOR_BASE,
              float *in_complex, uint64_t in_size,
              float *out_complex, uint64_t out_size);

    ~PlanFloat() override;

protected:
    void destroy_cpu_plan() override;

    void *get_fftw_plan(FFTW_PLAN_PARAMS) override;

    void destroy_gpu_plan() override;


};


#endif //RFFTC_PLANFLOAT_H
