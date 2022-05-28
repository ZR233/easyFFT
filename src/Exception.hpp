#include <stdexcept>
#include "include/easyFFT.h"


class Exception: public std::runtime_error{


public:
    Exception(const char * msg, FFT_ErrorCode error_code):
        std::runtime_error(msg),
        error_code(error_code){

    }

    const FFT_ErrorCode error_code;
};