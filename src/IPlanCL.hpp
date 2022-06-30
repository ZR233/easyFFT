#ifndef EASY_FFT_IPLAN_CL_HPP
#define EASY_FFT_IPLAN_CL_HPP


#include "IPlan.hpp"
#ifdef ENABLE_CL
#include "clFFT.h"
#endif
#include "Exception.hpp"
#ifdef ENABLE_CL
#include <iostream>
#include <sstream>

static void handle_cl_err(cl_int err) {
    switch (err) {
        case CL_SUCCESS:
            return;
        case CL_DEVICE_NOT_FOUND:
            throw Exception("", FFT_ERROR_CODE::DEVICE_NOT_FOUND);
        default:
            std::stringstream ss;
            ss << "(" << err << ")";
            throw Exception(ss.str().c_str(), FFT_ERROR_CODE::OPEN_CL);
    }
}
#endif
struct DeviceInfo {
    std::string name;
    uint32_t max_compute_units{};
};

template<typename T>
class IPlanCL : public IPlan<T> {
public:
    IPlanCL(FFTPlanConfig config,
              T *data_in, size_t data_in_size,
              T *data_out, size_t data_out_size) :
            IPlan<T>(config,
                     data_in, data_in_size,
                     data_out, data_out_size){

#ifdef ENABLE_CL

        clfftDim dim;
        switch (config.dim) {
            case 1:
                dim=CLFFT_1D;
                break;
            case 2:
                dim=CLFFT_2D;
                break;
            case 3:
                dim=CLFFT_3D;
                break;
            default:
                throw Exception("dim bigger than 3", FFT_ERROR_CODE::DIM_TOO_BIG);
        }
        std::vector<size_t> clLengths;
        byte_size= config.number_batches * sizeof (T);
        for (auto size: this->shape) {
            clLengths.push_back((size_t)size);
            byte_size*=size;
        }


        std::vector<std::shared_ptr<DeviceInfo>> device_infos;
        cl_uint platform_num;
        /* Setup OPEN_CL environment. */
        auto err = clGetPlatformIDs(0, nullptr, &platform_num);
        if (platform_num == 0) {
            throw Exception("", DEVICE_NOT_FOUND);
        }
        handle_cl_err(err);

        std::vector<cl_platform_id> platforms(platform_num);
        err = clGetPlatformIDs(platform_num, &platforms[0], nullptr);
        handle_cl_err(err);
        uint32_t max_compute_units = 0;

        for (auto platform_: platforms) {

            cl_uint device_num;
            err = clGetDeviceIDs(platform_, CL_DEVICE_TYPE_GPU, 0, nullptr, &device_num);
            if (err == CL_DEVICE_NOT_FOUND) {
                continue;
            }
            handle_cl_err(err);

            std::vector<cl_device_id> devices(device_num);
            err = clGetDeviceIDs(platform_, CL_DEVICE_TYPE_GPU, device_num, &devices[0], nullptr);
            handle_cl_err(err);

            for (auto device_id: devices) {
                auto info = getDeviceInfo(device_id);
                device_infos.push_back(info);

                if (info->max_compute_units > max_compute_units) {
                    platform=platform_;
                    device = device_id;
                    this->device_name = info->name;
                }
            }
        }

        props[1] = (cl_context_properties)platform;
        ctx = clCreateContext( props, 1, &device, nullptr, nullptr, &err );
        queue = clCreateCommandQueue( ctx, device, 0, &err );

        /* Setup clFFT. */
        clfftSetupData fftSetup;
        err = clfftInitSetupData(&fftSetup);
        handle_cl_err(err);

        err = clfftSetup(&fftSetup);
        handle_cl_err(err);

        /* Prepare OpenCL memory objects and place data inside them. */
        bufX = clCreateBuffer( ctx, CL_MEM_READ_WRITE, byte_size, nullptr, &err );

        /* Create a default plan for a complex FFT. */
        err = clfftCreateDefaultPlan(&planHandle, ctx, dim, clLengths.data());
        err = clfftSetPlanBatchSize(planHandle, config.number_batches);
        /* Set plan parameters. */
        err = clfftSetPlanPrecision(planHandle, CLFFT_SINGLE);
        err = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED);
        err = clfftSetResultLocation(planHandle, CLFFT_INPLACE);

        /* Bake the plan. */
        err = clfftBakePlan(planHandle, 1, &queue, nullptr, nullptr);
#else
        throw Exception("not_support_cl", FFT_ERROR_CODE::NOT_SUPPORT_CL);
#endif
    }

    virtual ~IPlanCL(){

        /* Release OpenCL memory objects. */
        clReleaseMemObject( bufX );

        /* Release the plan. */
        auto err = clfftDestroyPlan( &planHandle );
        handle_cl_err(err);
        /* Release clFFT library. */
        clfftTeardown( );

        /* Release OpenCL working objects. */
        clReleaseCommandQueue( queue );
        clReleaseContext( ctx );
    };

    void execute() override {
        auto err = clEnqueueWriteBuffer( queue, bufX, CL_TRUE, 0,
                                    byte_size, this->data_in, 0, NULL, NULL );
        handle_cl_err(err);

        clfftDirection dir;
        switch (this->config.sign) {
            case FFT_SIGN::FORWARD :
                dir=CLFFT_FORWARD;
                break;

            case FFT_SIGN::BACKWARD :
                dir=CLFFT_BACKWARD;
                break;
        }

        err = clfftEnqueueTransform(planHandle, dir, 1, &queue, 0,
                                    nullptr, nullptr, &bufX, nullptr, nullptr);
        handle_cl_err(err);

        /* Wait for calculations to be finished. */
        err = clFinish(queue);
        handle_cl_err(err);

        /* Fetch results of calculations. */
        err = clEnqueueReadBuffer( queue, bufX, CL_TRUE, 0, byte_size,
                                   this->data_out, 0, NULL, NULL );
        handle_cl_err(err);

    }

protected:
    cl_platform_id platform = nullptr;
    cl_device_id device = nullptr;
    cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
    cl_context ctx = nullptr;
    cl_command_queue queue = nullptr;
    cl_mem bufX={};
    clfftPlanHandle planHandle={};

private:
    size_t byte_size=0;
    static std::shared_ptr<DeviceInfo> getDeviceInfo(cl_device_id device) {
        auto info = std::make_shared<DeviceInfo>();
        char buffer[110];
        auto err = clGetDeviceInfo(device, CL_DEVICE_NAME, 100, buffer, nullptr);
        handle_cl_err(err);
        info->name.assign(buffer);
        cl_uint UnitNum;
        err = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &UnitNum, nullptr);
        handle_cl_err(err);
        info->max_compute_units = UnitNum;

        return info;
    }
};


#endif