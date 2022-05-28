
#ifndef Exception_HPP
#define Exception_HPP


#include <stdexcept>
#include "include/easyFFT.h"
#include <string>

class Exception: public std::runtime_error{


public:
    Exception(const char * msg, FFT_ERROR_CODE error_code):
        std::runtime_error(msg),
        error_code(error_code){
    }
    const FFT_ERROR_CODE error_code;
};

#endif