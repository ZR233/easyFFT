//
// Created by zrufo on 2022-05-26.
//

#ifndef RFFTC_PLANCOMPLEX_HPP
#define RFFTC_PLANCOMPLEX_HPP


#include "Plan.h"
#include <exception>

template<typename T>
class PlanComplex: public Plan{
public:
    PlanComplex (PLAN_CONSTRUCTOR_BASE,
                 T *in_complex, uint64_t in_size,
                 T *out_complex, uint64_t out_size):
        Plan(PLAN_CONSTRUCTOR_BASE_INPUT,
             in_complex, in_size, out_complex, out_size){

        auto need_size = 2;
        for (auto len: this->shape) {
            need_size *= len;
        }
        need_size *= numberBatches;

        if (need_size != in_size
            || need_size != out_size)

            throw std::length_error( "in_size error");
    }

private:
    PlanComplex()=default;
};



#endif //RFFTC_PLANCOMPLEX_HPP
