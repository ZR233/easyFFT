//
// Created by zrufo on 2022-05-15.
//
#include "include/easyFFT.h"
#include "PlanFFTWFloat.h"
#include "PlanCLFloat.h"
#include "Exception.hpp"
#include <cstring>

static void rest_result(struct Result *result){
    result->code = FFT_ERROR_CODE::OK;
    memset(result->msg, '\0', result->msg_size);
}

struct Result* fft_new_result(){
    auto result = new Result;
    result->msg_size = 2000;
    result->msg = new char [result->msg_size];
    rest_result(result);
    return result;
}
void fft_release_result(struct Result *result){
    delete [] result->msg;
    delete result;
}




FFTPlanFloat* fft_planf_init(
        struct FFTPlanConfig config,
        ComplexF *in_complex, uint64_t in_size,
        ComplexF *out_complex, uint64_t out_size,
        struct Result* result){

    rest_result(result);

    IPlan<ComplexF>* ptr;

    try{
        switch (config.device) {
            case FFT_DEVICE::CPU:
                ptr = new PlanFFTWFloat(config,
                                        in_complex, in_size,
                                        out_complex, out_size);
                break;
            case GPU:
                ptr = new PlanCLFloat(config,
                                        in_complex, in_size,
                                        out_complex, out_size);
                break;
        }


    }catch (Exception &e){
        result->code = e.error_code;
        strncpy(result->msg, e.what(), result->msg_size);
    }

    return (FFTPlanFloat*)ptr;
}
void fft_planf_device_name(FFTPlanFloat *plan, char *name, int size, struct Result* result){
    auto ptr = ( IPlan<ComplexF>*)plan;
    memset(name, '\0', size);
    ptr->device_name.copy(name, size);
    rest_result(result);
}

void fft_close_planf(FFTPlanFloat *plan){
    delete (IPlan<ComplexF>*)plan;
}

void fft_planf_execute(FFTPlanFloat *plan, struct Result* result){
    rest_result(result);

    try{
        auto ptr = (IPlan<ComplexF>*)plan;
        ptr->execute();
    }catch (Exception &e){
        result->code = e.error_code;
        strncpy(result->msg, e.what(), result->msg_size);
    }
}


void fft_close_plan(struct FFTPlanDouble *plan){

}