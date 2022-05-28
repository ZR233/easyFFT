//
// Created by zrufo on 2022-05-17.
//

#ifndef RFFTC_PLANFLOAT_H
#define RFFTC_PLANFLOAT_H

#include <complex>
#include "include/easyFFT.h"
#include <vector>
#include "Plan.hpp"

class PlanFloat : public Plan<ComplexF> {
public:
    PlanFloat(PLAN_CONSTRUCTOR_BASE,
              ComplexF *in_complex, uint64_t in_size,
              ComplexF *out_complex, uint64_t out_size);

    ~PlanFloat() override;

protected:
    void *get_fftw_plan(FFTW_PLAN_PARAMS) override;

    void cpu_plan_execute() override;
};


#endif //RFFTC_PLANFLOAT_H
