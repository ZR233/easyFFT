//
// Created by zrufo on 2022/6/29.
//

#ifndef EASYFFT_PLANCLFLOAT_H
#define EASYFFT_PLANCLFLOAT_H

#include "IPlanCL.hpp"


class PlanCLFloat: public IPlanCL<ComplexF> {
public:
    PlanCLFloat(FFTPlanConfig config,
                ComplexF *data_in, size_t data_in_size,
                ComplexF *data_out, size_t data_out_size);
};


#endif //EASYFFT_PLANCLFLOAT_H
