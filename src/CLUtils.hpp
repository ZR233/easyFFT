#ifndef EASY_FFT_CL_UTILS_HPP
#define EASY_FFT_CL_UTILS_HPP

#if VKFFT_BACKEND==3

#include <memory>
#include "CL/opencl.h"
#include <strstream>
#include "include/easyFFT.h"
#include "Exception.hpp"

struct DeviceInfo{
    std::string name;
    uint32_t max_compute_units;
};

static void handle_cl_err(cl_int err){
    if (err != CL_SUCCESS){
        std::strstream s;
        s << "CL: (" << err << ")";

        throw Exception(s.str(), CL);
    }
}

static  std::shared_ptr<DeviceInfo> getDeviceInfo(cl_device_id device){
    cl_int err ;
    auto info = std::make_shared<DeviceInfo>();
    char buffer[110];
    err = clGetDeviceInfo(device, CL_DEVICE_NAME, 100, buffer, nullptr);
    handle_cl_err(err);
    info->name.assign(buffer);
    cl_uint UnitNum;
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &UnitNum, NULL);
    handle_cl_err(err);
    info->max_compute_units = UnitNum;


    return info;
}

#endif
#endif


