#ifndef Exception_HPP
#define Exception_HPP

#include "include/easyFFT.h"
#include "vkFFT.h"
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
static void handle_vk_err(int err){
    if (err != VKFFT_SUCCESS){
        std::strstream s;
        s << "VKFFT error: (" << err << ")";

        throw Exception(s.str(), VKFFT);
    }
}
#endif