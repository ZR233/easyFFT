#ifndef EASY_FFT_IPLAN_FFTW_HPP
#define EASY_FFT_IPLAN_FFTW_HPP


#include "IPlan.hpp"
#include "fftw3.h"

struct FFTWParam{
    int rank;
    int *n; int howmany; const int *inembed; int istride;
    int idist; const int *onembed; int ostride; int odist; int sign;
    unsigned flags;
};



template<typename T, typename W>
class IPlanFFTW : public IPlan<T> {
public:
    IPlanFFTW(FFTPlanConfig config,
              T *data_in, size_t data_in_size,
              T *data_out, size_t data_out_size) :
            IPlan<T>(config,
                     data_in, data_in_size,
                     data_out, data_out_size) {

        this->device_name = "cpu";
        int ostride =1, istride = 1;
        int dist = 1;
        for (auto one: this->shape) {
            dist *= one;
        }

        fftw_params.rank = config.dim;
        fftw_params.n = this->shape.data();
        fftw_params.howmany = config.number_batches;
        fftw_params.istride = 1;
        fftw_params.ostride = 1;
        fftw_params.inembed = nullptr;
        fftw_params.onembed = nullptr;
        fftw_params.idist = dist;
        fftw_params.odist = dist;
        fftw_params.flags = FFTW_MEASURE;

        switch (config.sign) {
            case FORWARD:
                fftw_params.sign = FFTW_FORWARD;
                break;
            case BACKWARD:
                fftw_params.sign = FFTW_BACKWARD;
                break;
        }
    }

    virtual ~IPlanFFTW() = default;

protected:
    FFTWParam fftw_params={};
    W* plan;
};


#endif