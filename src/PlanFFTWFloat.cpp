//
// Created by zrufo on 2022/6/29.
//

#include "PlanFFTWFloat.h"

PlanFFTWFloat::PlanFFTWFloat(FFTPlanConfig config, ComplexF *data_in, size_t data_in_size, ComplexF *data_out,
                             size_t data_out_size):
        IPlanFFTW<ComplexF, fftwf_plan_s>(config, data_in, data_in_size, data_out, data_out_size){

    plan = fftwf_plan_many_dft(
            fftw_params.rank,
            fftw_params.n,
            fftw_params.howmany,
            data_in,
            fftw_params.inembed,
            fftw_params.istride,
            fftw_params.idist,
            data_out,
            fftw_params.onembed,
            fftw_params.ostride,
            fftw_params.odist,
            fftw_params.sign,
            fftw_params.flags
            );

}

void PlanFFTWFloat::execute() {
    fftwf_execute(plan);
}

PlanFFTWFloat::~PlanFFTWFloat() {
    fftwf_destroy_plan(plan);
}
