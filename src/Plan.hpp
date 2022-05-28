//
// Created by zrufo on 2022-05-26.
//

#ifndef EASY_FFT_PLAN_HPP
#define EASY_FFT_PLAN_HPP

#include <complex>
#include "include/easyFFT.h"
#include <vector>
#include <strstream>
#include "Exception.hpp"
#include "fftw3.h"
#include "vkFFT/vkFFT.h"

enum FFTW_PLAN_TYPE{
    FFTW_PLAN_TYPE_F,
    FFTW_PLAN_TYPE_D,
    FFTW_PLAN_TYPE_L,
};

#define FFTW_PLAN_PARAMS int rank, const int *n, int howmany, const int *inembed, int istride, \
    int idist, const int *onembed, int ostride, int odist, int sign, \
    unsigned flags

#define FFTW_PLAN_PARAMS_INPUT rank, n, howmany, in, inembed, istride, \
    idist, out, onembed, ostride, odist, sign, flags


template<typename T>
class Plan{
public:
    Plan (PLAN_CONSTRUCTOR_BASE, FFTW_PLAN_TYPE fftw_plan_type,
             T* data_in, uint64_t data_in_size,
             T* data_out, uint64_t data_out_size):
            sign(sign),
            _device(device),
            data_in(data_in),
            data_in_size(data_in_size),
            data_out(data_out),
            data_out_size(data_out_size),
            fftw_plan_type(fftw_plan_type),
            numberBatches(numberBatches){
        this->shape.assign(FFTDim, 0);

        for (int i = 0; i < FFTDim; ++i) {
            this->shape[i] = shape[i];
        }
    }
    virtual ~Plan() {
        switch (_device) {
            case FFT_DEVICE_CPU:
                destroy_cpu_plan();
                break;
            case FFT_DEVICE_GPU:
                destroy_gpu_plan();
                break;
        }
    }
    virtual void init(){
        auto need_size = 1;
        for (auto len: this->shape) {
            need_size *= len;
        }
        need_size *= numberBatches;

        if (need_size != data_in_size){
            std::strstream s;
            s << "need in size: "<< need_size << "actual: "<< data_in_size;

            throw Exception(s.str(), FFT_ERROR_CODE_IN_SIZE);
        }
        if (need_size != data_out_size){
            std::strstream s;
            s << "need out size: "<< need_size << "actual: "<< data_out_size;

            throw Exception(s.str(), FFT_ERROR_CODE_OUT_SIZE);
        }

        switch (_device) {
            case FFT_DEVICE_CPU:
                chose_fftw_plan();
                break;
            case FFT_DEVICE_GPU:

                break;
        }
    }
    void execute(){
        switch (_device) {
            case FFT_DEVICE_CPU:
                cpu_plan_execute();
                break;
            case FFT_DEVICE_GPU:
                gpu_plan_execute();
                break;
        }
    }
protected:

    std::vector<int> shape;
    int numberBatches;
    FFT_SIGN sign;
    FFT_DEVICE _device;


    void *originPlan = nullptr;


    T* data_in;
    const uint64_t data_in_size;
    T* data_out;
    const uint64_t data_out_size;
    const FFTW_PLAN_TYPE fftw_plan_type;

    virtual void cpu_plan_execute()=0;
    virtual void gpu_plan_execute()=0;

    virtual void *get_fftw_plan(FFTW_PLAN_PARAMS)=0;

private:
    void chose_fftw_plan(){
        int *inembed = shape.data(), *onembed = shape.data();
        int odist = 1;
        for (auto one: shape) {
            odist *= one;
        }
        int idist = odist;
        int ostride =1, istride = 1;
        int sign_;
        switch (this->sign) {
            case FFT_SIGN_FORWARD:
                sign_ = FFTW_FORWARD;
                break;
            case FFT_SIGN_BACKWARD:
                sign_ = FFTW_BACKWARD;
                break;
        }

        originPlan = get_fftw_plan(
                (int)shape.size(),
                shape.data(),
                numberBatches,
                inembed, istride, idist,
                onembed, ostride, odist,
                sign_,
                FFTW_MEASURE
        );
    }


#if VKFFT_BACKEND==3
    void handle_cl_err(cl_int err){
        if (err != CL_SUCCESS){
            std::strstream s;
            s << "CL: (" << err << ")";

            throw Exception(s.str(), FFT_ERROR_CODE_CL);
        }
    }

    // opencl
    void chose_gpu_plan(){
        cl_int err;
        cl_platform_id platform = nullptr;
        cl_device_id device = nullptr;
        cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
        cl_context ctx = nullptr;
        cl_command_queue queue = nullptr;
        cl_mem bufX;
        float *X;
        cl_event event = nullptr;
        int ret = 0;
        size_t N = 16;

        size_t clLengths[1] = {N};

        /* Setup OpenCL environment. */
        err = clGetPlatformIDs( 1, &platform, nullptr );
        handle_cl_err(err);

        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
        handle_cl_err(err);

        ctx = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err );
        handle_cl_err(err);

        queue = clCreateCommandQueue( ctx, device, 0, &err);

        handle_cl_err(err);
        VkFFTConfiguration configuration = {};
        VkFFTApplication app = {};

        if (shape.size() > 3){
            throw Exception("dim bigger than 3", FFT_ERROR_CODE_DIM_TOO_BIG);
        }

        configuration.FFTdim = shape.size(); //FFT dimension, 1D, 2D or 3D
        for (auto size : shape) {
            configuration.size[0] = size;
        }

        uint64_t bufferSize = (uint64_t)sizeof(float) * 2 * configuration.size[0];
        //Device management + code submission
        configuration.device = &device;
        configuration.context = &ctx;

        cl_mem buffer = nullptr;
//    buffer = clCreateBuffer(ctx, CL_MEM_READ_WRITE, bufferSize, nullptr, &err);
//    if (err != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_ALLOCATE;
//    configuration.buffer = &buffer;
//    configuration.bufferSize = &bufferSize;
//    configuration.numberBatches = 1;
//    err = clEnqueueWriteBuffer(queue, buffer, CL_TRUE, 0, bufferSize,
//                               buffer_input.data(), 0, nullptr, NULL);
//    if (err != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_COPY;
//
//    err = initializeVkFFT(&app, configuration);
//    if (err != VKFFT_SUCCESS) return err;
//
//    VkFFTLaunchParams launchParams = {};
//    launchParams.buffer = &buffer;
//
//    launchParams.commandQueue = &queue;
    }

#endif

#if VKFFT_BACKEND==0
    // vulkan
    void chose_gpu_plan(){

    }

#endif
    void destroy_cpu_plan(){
        if (originPlan != nullptr){
            switch (fftw_plan_type) {
                case FFTW_PLAN_TYPE_F:
                    fftwf_destroy_plan((fftwf_plan) originPlan);
                    break;
                case FFTW_PLAN_TYPE_D:
                    fftw_destroy_plan((fftw_plan) originPlan);
                    break;
                case FFTW_PLAN_TYPE_L:
                    fftwl_destroy_plan((fftwl_plan) originPlan);
                    break;
            }
        }
    }
    void destroy_gpu_plan(){

    }
};


#endif //RFFTC_PLAN_H
