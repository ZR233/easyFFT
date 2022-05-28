////
//// Created by zrufo on 2022-05-26.
////
//
//#ifndef RFFTC_PLANCOMPLEX_HPP
//#define RFFTC_PLANCOMPLEX_HPP
//
//
//#include "Plan.h"
//#include "Exception.hpp"
//#include <strstream>
//
//template<typename T>
//class PlanComplex: public Plan{
//public:
//    PlanComplex (PLAN_CONSTRUCTOR_BASE, FFTW_PLAN_TYPE fftw_plan_type,
//                 T *in_complex, uint64_t in_size,
//                 T *out_complex, uint64_t out_size):
//        Plan(PLAN_CONSTRUCTOR_BASE_INPUT, fftw_plan_type,
//             in_complex, in_size, out_complex, out_size){
//
//        auto need_size = 2;
//        for (auto len: this->shape) {
//            need_size *= len;
//        }
//        need_size *= numberBatches;
//
//        if (need_size != in_size){
//            std::strstream s;
//            s << "need in size: "<< need_size << "actual: "<< in_size;
//
//            throw Exception(s.str(), FFT_ERROR_CODE_IN_SIZE);
//        }
//        if (need_size != out_size){
//            std::strstream s;
//            s << "need out size: "<< need_size << "actual: "<< in_size;
//
//            throw Exception(s.str(), FFT_ERROR_CODE_OUT_SIZE);
//        }
//    }
//
//private:
//    PlanComplex()=default;
//};
//
//
//
//#endif //RFFTC_PLANCOMPLEX_HPP
