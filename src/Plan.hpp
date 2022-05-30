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
#include "vkFFT.h"
#include "CLUtils.hpp"


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
        need_size *= number_batches;

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
                chose_gpu_plan();
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

    virtual void cpu_plan_execute()=0;
    virtual void gpu_plan_execute(){
#if VKFFT_BACKEND==3
        auto err = clEnqueueWriteBuffer(queue, buffer, CL_TRUE, 0, buffer_size,
                                   data_in, 0, nullptr, NULL);
        handle_cl_err(err);
        VkFFTLaunchParams launchParams = {};
        launchParams.buffer = &buffer;

        launchParams.commandQueue = &queue;
        err = VkFFTAppend(vk_app.get(), -1, &launchParams);
        handle_vk_err(err);

/* Wait for calculations to be finished. */
        err = clFinish(queue);
        handle_cl_err(err);

        /* Fetch results of calculations. */
        err = clEnqueueReadBuffer( queue, buffer, CL_TRUE, 0,
                                   buffer_size, data_out, 0, nullptr, NULL );
        handle_cl_err(err);

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
                number_batches,
                inembed, istride, idist,
                onembed, ostride, odist,
                sign_,
                FFTW_MEASURE
        );
    }
    std::shared_ptr<VkFFTApplication>  vk_app;

    static void handle_vk_err(int err){
        if (err != VKFFT_SUCCESS){
            std::strstream s;
            s << "VKFFT error: (" << err << ")";

            throw Exception(s.str(), FFT_ERROR_CODE_VKFFT);
        }
    }


#if VKFFT_BACKEND==3
    cl_mem buffer= nullptr;
    uint64_t buffer_size = 0;
    cl_context ctx = nullptr;
    cl_command_queue queue = nullptr;
    // opencl
    void chose_gpu_plan(){
        cl_int err;
        cl_device_id device = nullptr;
        cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };

        cl_mem bufX;
        float *X;
        cl_event event = nullptr;
        int ret = 0;
        size_t N = 16;
        std::vector<std::shared_ptr<DeviceInfo>> device_infos;

        size_t clLengths[1] = {N};

        cl_uint platform_num;

        /* Setup OpenCL environment. */
        err = clGetPlatformIDs( 0, nullptr, &platform_num );
        if (err == CL_DEVICE_NOT_FOUND || platform_num==0){
            throw Exception("no openCL device", FFT_ERROR_CODE_NO_CL_DEVICE);
        }

        handle_cl_err(err);

        std::vector<cl_platform_id> platforms(platform_num);
        err = clGetPlatformIDs( platform_num, &platforms[0], nullptr);
        handle_cl_err(err);
        uint32_t max_compute_units = 0;

        for (auto platform: platforms) {
            cl_uint device_num;
            err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &device_num);
            if (err == CL_DEVICE_NOT_FOUND){
                continue;
            }
            handle_cl_err(err);

            std::vector<cl_device_id> devices(device_num);
            err = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU,device_num, &devices[0], nullptr);
            handle_cl_err(err);

            for (auto device_id: devices) {
                auto info = getDeviceInfo(device_id);
                device_infos.push_back(info);

                if (info->max_compute_units > max_compute_units){
                    device = device_id;
                    device_name = info->name;
                }
            }
        }


        ctx = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
        handle_cl_err(err);

        queue = clCreateCommandQueue( ctx, device, 0, &err);
        handle_cl_err(err);

        VkFFTConfiguration configuration = {};
        vk_app = std::make_shared<VkFFTApplication>();

        if (shape.size() > 3){
            throw Exception("dim bigger than 3", FFT_ERROR_CODE_DIM_TOO_BIG);
        }

        configuration.FFTdim = shape.size(); //FFT dimension, 1D, 2D or 3D
        for (auto size : shape) {
            configuration.size[0] = size;
        }

        buffer_size = sizeof(T) * number_batches;
        for (auto size: shape) {
            buffer_size *= size;
        }

        //Device management + code submission
        configuration.device = &device;
        configuration.context = &ctx;


        buffer = clCreateBuffer(ctx, CL_MEM_READ_WRITE, buffer_size, nullptr, &err);
        handle_cl_err(err);

        configuration.buffer = &buffer;
        configuration.bufferSize = &buffer_size;
        configuration.numberBatches = number_batches;

        err = initializeVkFFT(vk_app.get(), configuration);
        handle_vk_err(err);

//    err = clEnqueueWriteBuffer(queue, buffer, CL_TRUE, 0, bufferSize,
//                               buffer_input.data(), 0, nullptr, NULL);
//    if (err != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_COPY;
//

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
        /* Release OpenCL memory objects. */

#if VKFFT_BACKEND==3
        clReleaseMemObject( buffer);
        clReleaseCommandQueue( queue );
        clReleaseContext( ctx );
#endif

        deleteVkFFT(vk_app.get());
    }
};


#endif
