//
// Created by zrufo on 2022/6/29.
//

#include "PlanCLFloat.h"
#include "Exception.hpp"

PlanCLFloat::PlanCLFloat(FFTPlanConfig config, ComplexF *data_in, size_t data_in_size, ComplexF *data_out,
                         size_t data_out_size) : IPlanCL(config, data_in, data_in_size, data_out, data_out_size) {

}
