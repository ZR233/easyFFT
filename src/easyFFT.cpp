//
// Created by zrufo on 2022-05-15.
//
#include "include/easyFFT.h"
#include "PlanFloat.h"
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
enum FFT_ERROR_CODE fft_plan_device_name(FFTPlanFloat plan, char *name, int size){
    auto p = (PlanFloat*)plan.ptr;
    memset(name, '\0', size);
    p->device_name.copy(name, size);
    return FFT_ERROR_CODE_OK;
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