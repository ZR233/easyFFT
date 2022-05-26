//
// Created by zrufo on 2022-05-26.
//

#ifndef RFFTC_PLANCOMPLEX_HPP
#define RFFTC_PLANCOMPLEX_HPP


#include "Plan.h"

template<typename T>
class PlanComplex: public Plan{
public:
    PlanComplex PLAN_CONSTRUCTOR_PARAMS:
            Plan(FFTDim, shape, numberBatches, RFFT_PLAN_TYPE_COMPLEX_FLOAT, sign, device){

    }

private:
    PlanComplex()=default;

protected:
    void *getDataIn() override{
        return dataIn.data();
    }
    void *getDataOut() override{
        return dataOut.data();
    }

private:
    std::vector<std::complex<T>> dataIn;
    std::vector<std::complex<T>> dataOut;
};



#endif //RFFTC_PLANCOMPLEX_HPP
