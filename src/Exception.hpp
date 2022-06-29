#ifndef Exception_HPP
#define Exception_HPP

#include "include/easyFFT.h"

#include <string>
#include <stdexcept>

class Exception: public std::runtime_error{
public:
    Exception(const char * msg, FFT_ERROR_CODE error_code):
        runtime_error(msg),
        error_code(error_code){
    };

    const FFT_ERROR_CODE error_code;
};


//static void handle_vkfft_err(enum VkFFTResult err){
//
//    switch (err) {
//        case VKFFT_SUCCESS:
//            return;
//        case VKFFT_ERROR_MALLOC_FAILED:
//            throw Exception("", FFT_ERROR_CODE::MALLOC_FAILED);
//        default:
//            std::stringstream ss;
//            ss << "(" << err << ")";
//            throw Exception(ss.str().c_str(), FFT_ERROR_CODE::VKFFT);
//    }
//}
#endif