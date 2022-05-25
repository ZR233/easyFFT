//
// Created by zrufo on 2022-05-17.
//

#ifndef RFFTC_PLANFLOAT_H
#define RFFTC_PLANFLOAT_H

#include <complex>
#include "include/rfft.h"
#include <vector>

class Plan{
public:
    Plan(int FFTDim, const int *shape, int numberBatches, RFFT_PlanType planType, RFFT_SIGN sign);
    virtual ~Plan();

protected:
    RFFT_PlanType planType;
    bool isUseFFTW;
    std::vector<int> shape;
    int numberBatches;
    RFFT_SIGN sign;
    void * originPlan= nullptr;

    void chose_fftw_plan();
    virtual void * getDataIn()=0;
    virtual void * getDataOut()=0;
};



class PlanFloat: public Plan{
public:
    PlanFloat(int FFTDim, const int *shape, int numberBatches, RFFT_SIGN sign);
    ~PlanFloat() override;

private:
    PlanFloat()=default;

protected:
    void *getDataIn() override;

    void *getDataOut() override;

private:
    std::vector<std::complex<float>> dataIn;
    std::vector<std::complex<float>> dataOut;
};


#endif //RFFTC_PLANFLOAT_H
