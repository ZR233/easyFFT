//
// Created by zrufo on 2022-05-26.
//

#ifndef EASY_FFT_PLAN_HPP
#define EASY_FFT_PLAN_HPP

#include <complex>
#include "include/easyFFT.h"
#include <memory>
#include <vector>
#include <strstream>
#include "Exception.hpp"
#include "fftw3.h"
#include "vkFFT.h"
#include "CLUtils.hpp"
#include "utils/Device.hpp"
#if VKFFT_BACKEND==0
#include "utils/vulkan_tools.hpp"
#elif  VKFFT_BACKEND==3
#include "utils/opencl_tools.hpp"
#endif



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
    Plan (FFTPlanConfig config, FFTW_PLAN_TYPE fftw_plan_type,
             T* data_in, uint64_t data_in_size,
             T* data_out, uint64_t data_out_size):
            sign(config.sign),
            _device(config.device),
            data_in(data_in),
            data_in_size(data_in_size),
            data_out(data_out),
            data_out_size(data_out_size),
            fftw_plan_type(fftw_plan_type),
            number_batches(config.number_batches){
        this->shape.assign(config.dim, 0);

        for (int i = 0; i < config.dim; ++i) {
            this->shape[i] = config.shape[i];
        }
    }
    virtual ~Plan() {
        switch (_device) {
            case CPU:
                destroy_cpu_plan();
                break;
            case GPU:
                destroy_gpu_plan();
                break;
        }
    }
    virtual void init(){
        auto need_size = 1;
        for (auto len: this->shape) {
            need_size *= len;
        }
        need_size *= number_batches;

        if (need_size != data_in_size){
            std::strstream s;
            s << "need in size: "<< need_size << "actual: "<< data_in_size;

            throw Exception(s.str(), IN_SIZE);
        }
        if (need_size != data_out_size){
            std::strstream s;
            s << "need out size: "<< need_size << "actual: "<< data_out_size;

            throw Exception(s.str(), OUT_SIZE);
        }

        switch (_device) {
            case CPU:
                chose_fftw_plan();
                break;
            case GPU:
                chose_gpu_plan();
                break;
        }
    }
    void execute(){
        switch (_device) {
            case CPU:
                cpu_plan_execute();
                break;
            case GPU:
                gpu_plan_execute();
                break;
        }
    }
    std::string device_name;
protected:

    std::vector<int> shape;
    int number_batches;
    FFT_SIGN sign;
    FFT_DEVICE _device;
    void *originPlan = nullptr;
    T* data_in;
    const uint64_t data_in_size;
    T* data_out;
    const uint64_t data_out_size;
    const FFTW_PLAN_TYPE fftw_plan_type;
    easyfft::Device driver_device;
    virtual void cpu_plan_execute()=0;
    virtual void gpu_plan_execute(){
        auto inverse = 0;
        switch (sign) {
            case FORWARD:
                inverse = -1;
                break;
            case BACKWARD:
                inverse = 1;
                break;
        }
#if VKFFT_BACKEND==0
    easyfft::vulcan::execute(&driver_device, vk_app.get(), data_in, data_out, inverse);
#elif VKFFT_BACKEND==3
       easyfft::opencl::execute(&driver_device, vk_app.get(), data_in, data_out, inverse);
#endif
    }

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
            case FORWARD:
                sign_ = FFTW_FORWARD;
                break;
            case BACKWARD:
                sign_ = FFTW_BACKWARD;
                break;
        }

        originPlan = get_fftw_plan(
                (int)shape.size(),
                shape.data(),
                number_batches,
                inembed, istride, idist,
                onembed, ostride, odist,
                sign_,
                FFTW_MEASURE
        );
    }
    std::shared_ptr<VkFFTApplication>  vk_app;


    void chose_gpu_plan() {
        VkFFTConfiguration configuration = {};
        vk_app = std::make_shared<VkFFTApplication>();

        if (shape.size() > 3){
            throw Exception("dim bigger than 3", DIM_TOO_BIG);
        }

        configuration.FFTdim = shape.size(); //FFT dimension, 1D, 2D or 3D
        for (auto size : shape) {
            configuration.size[0] = size;
        }

        driver_device.buffer_size = sizeof(T) * number_batches;
        for (auto size: shape) {
            driver_device.buffer_size *= size;
        }


        configuration.bufferSize = &driver_device.buffer_size;
        configuration.numberBatches = number_batches;


#if VKFFT_BACKEND == 0 // vulkan
       easyfft::vulcan::initDevice(&driver_device, configuration);

#elif VKFFT_BACKEND == 3 // opencl
        easyfft::opencl::initDevice(&driver_device, configuration);
#endif


        auto err = initializeVkFFT(vk_app.get(), configuration);
        handle_vkfft_err(err);
    }

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
//                    fftwl_destroy_plan((fftwl_plan) originPlan);
                    break;
            }
        }
    }
    void destroy_gpu_plan(){
        /* Release OPEN_CL memory objects. */

#if VKFFT_BACKEND==3
        easyfft::opencl::closeDevice(&driver_device);
#endif

        deleteVkFFT(vk_app.get());
    }
};


#endif
