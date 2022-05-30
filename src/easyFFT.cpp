//
// Created by zrufo on 2022-05-15.
//
#include "include/easyFFT.h"
#include "PlanFloat.h"
#include "Exception.hpp"




enum FFT_ERROR_CODE fft_plan_init(
        struct FFTPlanFloat *plan,
                ComplexF* in_complex, uint64_t in_size,
                ComplexF* out_complex, uint64_t out_size){

    auto err = FFT_ERROR_CODE_OK;

    auto ptr = new PlanFloat(plan->config,
                             in_complex, in_size,
                             out_complex, out_size);

    try{
        ptr->init();
    }catch (Exception &e){
        err = e.error_code;
    }

    plan->ptr = ptr;
    return err;
}
enum FFT_ERROR_CODE fft_plan_device_name(FFTPlanFloat *plan, char *name, int size){
    auto p = (PlanFloat*)plan->ptr;
    memset(name, '\0', size);
    p->device_name.copy(name, size);
    return FFT_ERROR_CODE_OK;
}

void fft_close_plan(FFTPlanFloat *plan){
    delete (PlanFloat*)plan->ptr;
}

enum FFT_ERROR_CODE fft_execute(FFTPlanFloat *plan){
    try{
        auto ptr = (PlanFloat*)plan->ptr;
        ptr->execute();
    }catch (Exception &e){
        return e.error_code;
    }
    return FFT_ERROR_CODE_OK;
}