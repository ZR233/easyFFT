//
// Created by zrufo on 2022-05-15.
//

#ifndef RFFT_RFFT_H
#define RFFT_RFFT_H

#include <stdint.h>

#ifdef WIN32
#define EXT __declspec(dllexport)
#else
#define EXT
#endif


enum RFFT_ErrorCode{
    RFFT_ErrorCode_OK,
    RFFT_ErrorCode_PLAN_TYPE
};


enum RFFT_PlanType{
    RFFT_PLAN_TYPE_COMPLEX_FLOAT,
    RFFT_PLAN_TYPE_COMPLEX_DOUBLE
};
enum RFFT_SIGN{
    RFFT_SIGN_FORWARD,
    RFFT_SIGN_BACKWARD,
};

#ifdef __cplusplus
extern "C" {
#endif
int hello();



EXT uint64_t createPlan(RFFT_PlanType type, RFFT_ErrorCode* err);
EXT void closePlan(uint64_t ptr);

#ifdef __cplusplus
}
#endif


#endif //RFFT_RFFT_H


