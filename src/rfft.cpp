//
// Created by zrufo on 2022-05-15.
//
#include "include/rfft.h"
#include "vkFFT/vkFFT.h"
#include "PlanFloat.h"
#include <iostream>


int hello(){
    std::cout<< VKFFT_BACKEND<< std::endl;

   return VKFFT_BACKEND;
}


uint64_t createPlan(RFFT_PlanType type, RFFT_ErrorCode* err){
    *err = RFFT_ErrorCode_OK;
    Plan* ptr = nullptr;
    switch (type) {
        case RFFT_PLAN_TYPE_COMPLEX_FLOAT:
//            ptr = new PlanFloat(1);
            break;

        default:
            *err = RFFT_ErrorCode_PLAN_TYPE;
            return 0;
    }

    return (uint64_t) ptr;
}


void closePlan(uint64_t ptr){
    delete (PlanFloat*)ptr;
}