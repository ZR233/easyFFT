//
// Created by zrufo on 2022-05-15.
//
#include "include/easyFFT.h"
#include "vkFFT/vkFFT.h"
#include "PlanFloat.h"
#include <iostream>
#include "Exception.hpp"




FFTPlanFloat fft_new_plan_float(PLAN_CONSTRUCTOR_BASE,
                                ComplexF* in_complex, uint64_t in_size,
                                ComplexF* out_complex, uint64_t out_size,
                                enum FFT_ERROR_CODE* err){
    *err = FFT_ERROR_CODE_OK;
    FFTPlanFloat plan{};
    auto ptr = new PlanFloat(PLAN_CONSTRUCTOR_BASE_INPUT,
                             in_complex, in_size,
                             out_complex, out_size);

    *err = FFT_ERROR_CODE_OK;
    try{
        ptr->init();
    }catch (Exception &e){
        *err = e.error_code;
    }

    plan.ptr = ptr;
    return plan;
}


void fft_close_plan(FFTPlanFloat plan){
    delete (PlanFloat*)plan.ptr;
}

enum FFT_ERROR_CODE fft_execute(FFTPlanFloat plan){
    try{
        auto ptr = (PlanFloat*)plan.ptr;
        ptr->execute();
    }catch (Exception &e){
        return e.error_code;
    }
    return FFT_ERROR_CODE_OK;
}