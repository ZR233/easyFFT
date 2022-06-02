
#ifndef Exception_HPP
#define Exception_HPP


#include <exception>
#include "include/easyFFT.h"
#include <string>

class Exception: public std::exception{
public:
    Exception(const char * msg, FFT_ERROR_CODE error_code):
        std::exception(),
        error_code(error_code),
        msg(msg){
    }
    const FFT_ERROR_CODE error_code;
    const char * what() const noexcept override{
        return msg.data();
    }

private:
    std::string msg;
};

#endif