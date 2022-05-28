//
// Created by zrufo on 2022-05-15.
//
#include "include/easyFFT.h"
#include "vkFFT/vkFFT.h"
#include "PlanFloat.h"
#include <iostream>
#include "Exception.hpp"




FFTPlanFloat fft_new_plan_float(PLAN_CONSTRUCTOR_BASE,
                                float* in_complex, uint64_t in_size,
                                float* out_complex, uint64_t out_size,
                                FFT_ErrorCode* err){
    *err = FFT_ErrorCode_OK;
    FFTPlanFloat plan{};
    plan.ptr = new PlanFloat(PLAN_CONSTRUCTOR_BASE_INPUT,
                             in_complex, in_size,
                             out_complex, out_size);
    return plan;
}


void fft_close_plan(FFTPlanFloat plan){
    delete (PlanFloat*)plan.ptr;
}

FFT_ErrorCode fft_execute(FFTPlanFloat plan){
    try{
        auto ptr = (PlanFloat*)plan.ptr;
        ptr->execute();
    }catch (Exception &e){
        return e.error_code;
    }
    return FFT_ErrorCode_OK;
}