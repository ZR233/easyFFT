#ifndef EASY_FFT_IPLAN_HPP
#define EASY_FFT_IPLAN_HPP


#include <complex>
#include "include/easyFFT.h"
#include <vector>
#include <string>

template<typename T>
class IPlan{
public:
    IPlan (FFTPlanConfig config,
           T* data_in, size_t data_in_size,
           T* data_out, size_t data_out_size):
            config(config),
            data_in(data_in),
            data_in_size(data_in_size),
            data_out(data_out),
            data_out_size(data_out_size){

        shape=std::vector<int>(config.dim);
        for (int i = 0; i < config.dim; ++i) {
            shape[i]=(int)config.shape[i];
        }

    }

    virtual ~IPlan() = default;

    virtual void execute()=0;
    std::string device_name="unknown";
protected:
    T* data_in;
    size_t data_in_size;
    T* data_out;
    size_t data_out_size;
    FFTPlanConfig config;
    std::vector<int> shape;
};



#endif